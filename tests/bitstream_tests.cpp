#include "BitStream.h"
#include "Capacity.h"
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>

static unsigned checks = 0;
static void Check(bool value)
{
    ++checks;
    if (!value) throw std::runtime_error("Check failed at assertion " + std::to_string(checks));
}

template<class Function> static void Reject(Function run)
{
    bool rejected = false;
    try { run(); } catch (const std::exception&) { rejected = true; }
    Check(rejected);
}

static void RoundTrip(uint8_t bytesPerElement, uint8_t depth)
{
    const auto pattern = BitStream::GetByteLevelBitDepths(depth, bytesPerElement);
    std::vector<uint8_t> carrier(512 * bytesPerElement, 0xa5);
    const auto original = carrier;
    std::vector<uint8_t> payload(static_cast<std::size_t>(CarrierCapacityBytes(512, bytesPerElement, depth)));
    for (std::size_t i = 0; i < payload.size(); ++i) payload[i] = uint8_t(i * 73 + 19);
    BitStream writer(carrier, pattern);
    writer.WriteBytes(payload); // Exactly fills the usable slots.
    Check(writer.AtEnd());
    const auto written = carrier;
    Reject([&] { writer.WriteBits(1, 1); });
    Check(carrier == written);
    BitStream reader(carrier, pattern);
    Check(reader.ReadBytes(payload.size()) == payload);
    Reject([&] { reader.ReadBits(1); });

    // Independent slot-by-slot oracle: checks byte traversal, payload order and untouched high bits.
    uint64_t payloadBit = 0;
    for (std::size_t i = 0; i < carrier.size(); ++i)
    {
        const auto byteDepth = pattern[i % pattern.size()];
        const auto mask = byteDepth == 0 ? 0u : (1u << byteDepth) - 1u;
        Check((carrier[i] & ~mask) == (original[i] & ~mask));
        for (uint8_t bit = 0; bit < byteDepth; ++bit, ++payloadBit)
            Check(((carrier[i] >> bit) & 1u) == ((payload[payloadBit / 8] >> (payloadBit % 8)) & 1u));
    }
    BitStream tooLarge(carrier, pattern);
    payload.push_back(0);
    Reject([&] { tooLarge.WriteBytes(payload); });
    Check(carrier == written);
}

int main()
{
    try
    {
        for (uint8_t depth = 1; depth <= 8; ++depth) RoundTrip(1, depth);
        for (int depth : {1, 2, 7, 8, 9, 12, 16}) RoundTrip(2, static_cast<uint8_t>(depth));
        for (int depth : {1, 2, 8, 9, 16, 24, 31, 32}) RoundTrip(4, static_cast<uint8_t>(depth));
        Check(CarrierCapacityBytes(1000, 4, 1) == 125);
        Check(CarrierCapacityBytes(7, 2, 1) == 0);
        Check(CarrierCapacityBytes(100, 4, 0) == 0);
        Reject([] { CarrierCapacityBytes(UINT64_MAX, 4, 32); });
        Reject([] { BitStream::GetByteLevelBitDepths(9, 1); });
        std::vector<uint8_t> bytes(2048, 0);
        Reject([&] { BitStream stream(bytes, {}); });
        Reject([&] { BitStream stream(bytes, {0, 0}); });
        Reject([&] { BitStream stream(bytes, {9}); });

        BitStream bitWriter(bytes, {1, 0, 0, 0});
        bitWriter.WriteBits(5, 3);
        bitWriter.WriteBits(0xad, 8);
        BitStream bitReader(bytes, {1, 0, 0, 0});
        Check(bitReader.ReadBits(3) == 5);
        Check(bitReader.ReadBits(8) == 0xad);

        Container container;
        container.files.emplace_back("hello.txt", std::vector<uint8_t>{'h', 'i'});
        container.files.emplace_back("bytes.bin", std::vector<uint8_t>{0, 255, 1, 128});
        container.files.emplace_back("empty.dat", std::vector<uint8_t>{});
        std::vector<uint8_t> exact(static_cast<std::size_t>(container.GetSize()), 0);
        BitStream writer(exact, {8});
        writer.WriteContainer(container);
        Check(writer.AtEnd());
        Check(exact[9] == 3 && exact[10] == 0); // Explicit little-endian file count.
        BitStream reader(exact, {8});
        std::unique_ptr<Container> result(reader.ReadContainer());
        Check(result && result->files.size() == 3);
        for (std::size_t i = 0; i < 3; ++i)
        {
            Check(result->files[i].fileName == container.files[i].fileName);
            Check(result->files[i].data == container.files[i].data);
        }
        exact.resize(exact.size() - 1);
        BitStream truncated(exact, {8});
        Reject([&] { std::unique_ptr<Container> invalid(truncated.ReadContainer()); });
        BitStream oversized(exact, {8});
        Reject([&] { oversized.WriteContainer(container); });

        for (bool badCount : {true, false})
        {
            std::vector<uint8_t> malformed(40, 0);
            BitStream invalidWriter(malformed, {8});
            invalidWriter.WriteKey();
            invalidWriter.WriteUInt64(badCount ? UINT64_MAX : 1);
            if (!badCount) invalidWriter.WriteUInt64(UINT64_MAX);
            BitStream invalidReader(malformed, {8});
            Reject([&] { std::unique_ptr<Container> invalid(invalidReader.ReadContainer()); });
        }
        std::vector<uint8_t> badDataLength(40, 0);
        BitStream dataLengthWriter(badDataLength, {8});
        dataLengthWriter.WriteKey();
        dataLengthWriter.WriteUInt64(1);
        dataLengthWriter.WriteString("");
        dataLengthWriter.WriteUInt64(UINT64_MAX);
        BitStream dataLengthReader(badDataLength, {8});
        Reject([&] { std::unique_ptr<Container> invalid(dataLengthReader.ReadContainer()); });
        std::vector<uint8_t> noContainer(20, 0);
        BitStream empty(noContainer, {8});
        Check(empty.ReadContainer() == nullptr);
        std::cout << "Core tests passed: " << checks << " assertions across 23 carrier depth cases.\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
