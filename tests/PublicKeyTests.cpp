// Copyright © 2017-2022 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "PublicKey.h"

#include "Hash.h"
#include "HexCoding.h"
#include "PrivateKey.h"
#include "interface/TWTestUtilities.h"

#include <gtest/gtest.h>

using namespace TW;

TEST(PublicKeyTests, CreateFromPrivateSecp256k1) {
    const Data key = parse_hex("afeefca74d9a325cf1d6b6911d61a65c32afa8e02bd5e78e2e4ac2910bab45f5");
    auto privateKey = PrivateKey(key);
    auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);
    EXPECT_EQ(publicKey.bytes.size(), 33);
    EXPECT_EQ(hex(publicKey.bytes), "0399c6f51ad6f98c9c583f8e92bb7758ab2ca9a04110c0a1126ec43e5453d196c1");
    EXPECT_EQ(publicKey.isCompressed(), true);
    EXPECT_TRUE(PublicKey::isValid(publicKey.bytes, TWPublicKeyTypeSECP256k1));
}

TEST(PublicKeyTests, CreateFromDataSecp256k1) {
    const Data key = parse_hex("0399c6f51ad6f98c9c583f8e92bb7758ab2ca9a04110c0a1126ec43e5453d196c1");
    PublicKey publicKey(key, TWPublicKeyTypeSECP256k1);
    EXPECT_EQ(hex(publicKey.bytes), hex(key));
}

TEST(PublicKeyTests, CreateInvalid) {
    const Data keyInvalid = parse_hex("afeefca74d9a325cf1d6b6911d61a65c32af"); // too short
    try {
        PublicKey publicKey(keyInvalid, TWPublicKeyTypeSECP256k1);
    } catch (const std::invalid_argument&) {
        return; // OK
    }
    FAIL() << "Missing expected exception";
}

TEST(PublicKeyTests, CreateBlake) {
    const auto privateKeyHex = "afeefca74d9a325cf1d6b6911d61a65c32afa8e02bd5e78e2e4ac2910bab45f5";
    const auto publicKeyKeyHex = "b689ab808542e13f3d2ec56fe1efe43a1660dcadc73ce489fde7df98dd8ce5d9";
    {
        auto publicKey = PrivateKey(parse_hex(privateKeyHex)).getPublicKey(TWPublicKeyTypeED25519Blake2b);
        EXPECT_EQ(hex(publicKey.bytes), publicKeyKeyHex);
        EXPECT_EQ(publicKey.bytes.size(), 32);
    }
    {
        const auto publicKey = PublicKey(parse_hex(publicKeyKeyHex), TWPublicKeyTypeED25519Blake2b);
        EXPECT_EQ(hex(publicKey.bytes), publicKeyKeyHex);
    }
}

TEST(PublicKeyTests, CompressedExtended) {
    const Data key = parse_hex("afeefca74d9a325cf1d6b6911d61a65c32afa8e02bd5e78e2e4ac2910bab45f5");
    auto privateKey = PrivateKey(key);
    auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);
    EXPECT_EQ(publicKey.type, TWPublicKeyTypeSECP256k1);
    EXPECT_EQ(publicKey.bytes.size(), 33);
    EXPECT_EQ(publicKey.isCompressed(), true);
    EXPECT_TRUE(PublicKey::isValid(publicKey.bytes, TWPublicKeyTypeSECP256k1));
    EXPECT_EQ(hex(publicKey.bytes), std::string("0399c6f51ad6f98c9c583f8e92bb7758ab2ca9a04110c0a1126ec43e5453d196c1"));

    auto extended = publicKey.extended();
    EXPECT_EQ(extended.type, TWPublicKeyTypeSECP256k1Extended);
    EXPECT_EQ(extended.bytes.size(), 65);
    EXPECT_EQ(extended.isCompressed(), false);
    EXPECT_TRUE(PublicKey::isValid(extended.bytes, TWPublicKeyTypeSECP256k1Extended));
    EXPECT_EQ(hex(extended.bytes), std::string("0499c6f51ad6f98c9c583f8e92bb7758ab2ca9a04110c0a1126ec43e5453d196c166b489a4b7c491e7688e6ebea3a71fc3a1a48d60f98d5ce84c93b65e423fde91"));

    auto compressed = extended.compressed();
    EXPECT_EQ(compressed.type, TWPublicKeyTypeSECP256k1);
    EXPECT_TRUE(compressed == publicKey);
    EXPECT_EQ(compressed.bytes.size(), 33);
    EXPECT_EQ(compressed.isCompressed(), true);
    EXPECT_TRUE(PublicKey::isValid(compressed.bytes, TWPublicKeyTypeSECP256k1));
    EXPECT_EQ(hex(compressed.bytes), std::string("0399c6f51ad6f98c9c583f8e92bb7758ab2ca9a04110c0a1126ec43e5453d196c1"));

    auto extended2 = extended.extended();
    EXPECT_EQ(extended2.type, TWPublicKeyTypeSECP256k1Extended);
    EXPECT_EQ(extended2.bytes.size(), 65);
    EXPECT_EQ(extended2.isCompressed(), false);

    auto compressed2 = compressed.compressed();
    EXPECT_EQ(compressed2.type, TWPublicKeyTypeSECP256k1);
    EXPECT_TRUE(compressed2 == publicKey);
    EXPECT_EQ(compressed2.bytes.size(), 33);
    EXPECT_EQ(compressed2.isCompressed(), true);
}

TEST(PublicKeyTests, CompressedExtendedNist) {
    const Data key = parse_hex("afeefca74d9a325cf1d6b6911d61a65c32afa8e02bd5e78e2e4ac2910bab45f5");
    auto privateKey = PrivateKey(key);
    auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeNIST256p1);
    EXPECT_EQ(publicKey.type, TWPublicKeyTypeNIST256p1);
    EXPECT_EQ(publicKey.bytes.size(), 33);
    EXPECT_EQ(publicKey.isCompressed(), true);
    EXPECT_TRUE(PublicKey::isValid(publicKey.bytes, TWPublicKeyTypeNIST256p1));
    EXPECT_EQ(hex(publicKey.bytes), std::string("026d786ab8fda678cf50f71d13641049a393b325063b8c0d4e5070de48a2caf9ab"));

    auto extended = publicKey.extended();
    EXPECT_EQ(extended.type, TWPublicKeyTypeNIST256p1Extended);
    EXPECT_EQ(extended.bytes.size(), 65);
    EXPECT_EQ(extended.isCompressed(), false);
    EXPECT_TRUE(PublicKey::isValid(extended.bytes, TWPublicKeyTypeNIST256p1Extended));
    EXPECT_EQ(hex(extended.bytes), std::string("046d786ab8fda678cf50f71d13641049a393b325063b8c0d4e5070de48a2caf9ab918b4fe46ccbf56701fb210d67d91c5779468f6b3fdc7a63692b9b62543f47ae"));

    auto compressed = extended.compressed();
    EXPECT_EQ(compressed.type, TWPublicKeyTypeNIST256p1);
    EXPECT_TRUE(compressed == publicKey);
    EXPECT_EQ(compressed.bytes.size(), 33);
    EXPECT_EQ(compressed.isCompressed(), true);
    EXPECT_TRUE(PublicKey::isValid(compressed.bytes, TWPublicKeyTypeNIST256p1));
    EXPECT_EQ(hex(compressed.bytes), std::string("026d786ab8fda678cf50f71d13641049a393b325063b8c0d4e5070de48a2caf9ab"));

    auto extended2 = extended.extended();
    EXPECT_EQ(extended2.type, TWPublicKeyTypeNIST256p1Extended);
    EXPECT_EQ(extended2.bytes.size(), 65);
    EXPECT_EQ(extended2.isCompressed(), false);

    auto compressed2 = compressed.compressed();
    EXPECT_EQ(compressed2.type, TWPublicKeyTypeNIST256p1);
    EXPECT_TRUE(compressed2 == publicKey);
    EXPECT_EQ(compressed2.bytes.size(), 33);
    EXPECT_EQ(compressed2.isCompressed(), true);
}

TEST(PublicKeyTests, CompressedExtendedED25519) {
    const Data key = parse_hex("afeefca74d9a325cf1d6b6911d61a65c32afa8e02bd5e78e2e4ac2910bab45f5");
    auto privateKey = PrivateKey(key);
    auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeED25519);
    EXPECT_EQ(publicKey.type, TWPublicKeyTypeED25519);
    EXPECT_EQ(publicKey.bytes.size(), 32);
    EXPECT_EQ(publicKey.isCompressed(), true);
    EXPECT_TRUE(PublicKey::isValid(publicKey.bytes, TWPublicKeyTypeED25519));
    EXPECT_EQ(hex(publicKey.bytes), std::string("4870d56d074c50e891506d78faa4fb69ca039cc5f131eb491e166b975880e867"));

    auto extended = publicKey.extended();
    EXPECT_EQ(extended.type, TWPublicKeyTypeED25519);
    EXPECT_TRUE(extended == publicKey);
    EXPECT_EQ(extended.bytes.size(), 32);
    EXPECT_EQ(extended.isCompressed(), true);

    auto compressed = publicKey.compressed();
    EXPECT_EQ(compressed.type, TWPublicKeyTypeED25519);
    EXPECT_TRUE(compressed == publicKey);
    EXPECT_EQ(compressed.bytes.size(), 32);
    EXPECT_EQ(compressed.isCompressed(), true);
}

TEST(PublicKeyTests, IsValidWrongType) {
    EXPECT_FALSE(PublicKey::isValid(parse_hex("deadbeef"), (enum TWPublicKeyType)99));
}

TEST(PublicKeyTests, Verify) {
    const auto privateKey = PrivateKey(parse_hex("afeefca74d9a325cf1d6b6911d61a65c32afa8e02bd5e78e2e4ac2910bab45f5"));

    const char* message = "Hello";
    const Data messageData = TW::data(message);
    const Data digest = Hash::sha256(messageData);

    {
        const auto signature = privateKey.sign(digest, TWCurveSECP256k1);
        const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);
        EXPECT_TRUE(publicKey.verify(signature, digest));
        EXPECT_EQ(hex(signature), "0f5d5a9e5fc4b82a625312f3be5d3e8ad017d882de86c72c92fcefa924e894c12071772a14201a3a0debf381b5e8dea39fadb9bcabdc02ee71ab018f55bf717f01");
    }
    {
        const auto signature = privateKey.sign(digest, TWCurveED25519);
        const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeED25519);
        EXPECT_TRUE(publicKey.verify(signature, digest));
        EXPECT_EQ(hex(signature), "42848abf2641a731e18b8a1fb80eff341a5acebdc56faeccdcbadb960aef775192842fccec344679446daa4d02d264259c8f9aa364164ebe0ebea218581e2e03");
    }
    {
        const auto signature = privateKey.sign(digest, TWCurveED25519Blake2bNano);
        const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeED25519Blake2b);
        EXPECT_TRUE(publicKey.verify(signature, digest));
        EXPECT_EQ(hex(signature), "5c1473944cd0234ebc5a91b2966b9e707a33b936dadd149417a2e53b6b3fc97bef17b767b1690708c74d7b4c8fe48703fd44a6ef59d4cc5b9f88ba992db0a003");
    }
    {
        const auto signature = privateKey.sign(digest, TWCurveNIST256p1);
        const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeNIST256p1Extended);
        EXPECT_TRUE(publicKey.verify(signature, digest));
        EXPECT_EQ(hex(signature), "2e4655831f0c60729583595c103bf0d862af6313e4326f03f512682106c792822f5a9cd21e7d4a3316c2d337e5eee649b09c34f7b4407344f0d32e8d33167d8901");
    }
}

TEST(PublicKeyTests, VerifyAsDER) {
    const auto privateKey = PrivateKey(parse_hex("afeefca74d9a325cf1d6b6911d61a65c32afa8e02bd5e78e2e4ac2910bab45f5"));

    const char* message = "Hello";
    const Data messageData = TW::data(message);
    const Data digest = Hash::sha256(messageData);

    const auto signature = privateKey.signAsDER(digest, TWCurveSECP256k1);
    EXPECT_EQ(signature.size(), 70);
    EXPECT_EQ(hex(signature), "304402200f5d5a9e5fc4b82a625312f3be5d3e8ad017d882de86c72c92fcefa924e894c102202071772a14201a3a0debf381b5e8dea39fadb9bcabdc02ee71ab018f55bf717f");

    const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);
    EXPECT_EQ(hex(publicKey.bytes), "0399c6f51ad6f98c9c583f8e92bb7758ab2ca9a04110c0a1126ec43e5453d196c1");

    EXPECT_TRUE(publicKey.verifyAsDER(signature, digest));

    EXPECT_FALSE(publicKey.verify(signature, digest));

    { // Negative: wrong key type
        const auto publicKeyWrong = privateKey.getPublicKey(TWPublicKeyTypeNIST256p1Extended);
        EXPECT_FALSE(publicKeyWrong.verifyAsDER(signature, digest));
    }
}

// TEST(PublicKeyTests, VerifyEd25519Extended) {
//     const auto privateKey = PrivateKey(parse_hex("afeefca74d9a325cf1d6b6911d61a65c32afa8e02bd5e78e2e4ac2910bab45f5"));

//     const Data messageData = TW::data("Hello");
//     const Data digest = Hash::sha256(messageData);

//     try {
//         privateKey.sign(digest, TWCurveED25519Extended);
//     } catch (const std::invalid_argument&) {
//         return; // OK, not implemented
//     }
//     FAIL() << "Missing expected exception";
// }

TEST(PublicKeyTests, VerifySchnorr) {
    const auto key = PrivateKey(parse_hex("afeefca74d9a325cf1d6b6911d61a65c32afa8e02bd5e78e2e4ac2910bab45f5"));
    const auto privateKey = PrivateKey(key);

    const Data messageData = TW::data("hello schnorr");
    const Data digest = Hash::sha256(messageData);

    const auto signature = privateKey.signSchnorr(digest, TWCurveSECP256k1);
    const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);
    EXPECT_TRUE(publicKey.verifySchnorr(signature, digest));
    EXPECT_EQ(hex(signature), "b8118ccb99563fe014279c957b0a9d563c1666e00367e9896fe541765246964f64a53052513da4e6dc20fdaf69ef0d95b4ca51c87ad3478986cf053c2dd0b853");
}

TEST(PublicKeyTests, VerifySchnorrWrongType) {
    const auto key = PrivateKey(parse_hex("afeefca74d9a325cf1d6b6911d61a65c32afa8e02bd5e78e2e4ac2910bab45f5"));
    const auto privateKey = PrivateKey(key);

    const Data messageData = TW::data("hello schnorr");
    const Data digest = Hash::sha256(messageData);

    const auto signature = privateKey.signSchnorr(digest, TWCurveSECP256k1);
    const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeNIST256p1);
    EXPECT_FALSE(publicKey.verifySchnorr(signature, digest));
}

TEST(PublicKeyTests, Recover) {
    {
        const auto message = parse_hex("de4e9524586d6fce45667f9ff12f661e79870c4105fa0fb58af976619bb11432");
        const auto signature = parse_hex("00000000000000000000000000000000000000000000000000000000000000020123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef80");
        const auto publicKey = PublicKey::recover(signature, message);
        EXPECT_EQ(publicKey.type, TWPublicKeyTypeSECP256k1Extended);
        EXPECT_EQ(hex(publicKey.bytes),
            "0456d8089137b1fd0d890f8c7d4a04d0fd4520a30b19518ee87bd168ea12ed8090329274c4c6c0d9df04515776f2741eeffc30235d596065d718c3973e19711ad0");
    }

    const auto privateKey = PrivateKey(parse_hex("4f96ed80e9a7555a6f74b3d658afdd9c756b0a40d4ca30c42c2039eb449bb904"));
    const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1Extended);
    EXPECT_EQ(hex(publicKey.bytes), "0463ade8ebc212b85e7e4278dc3dcb4f9cc18aab912ef5d302b5d1940e772e9e1a9213522efddad487bbd5dd7907e8e776f918e9a5e4cb51893724e9fe76792a4f");
    {
        const auto message = parse_hex("6468eb103d51c9a683b51818fdb73390151c9973831d2cfb4e9587ad54273155");
        const auto signature = parse_hex("92c336138f7d0231fe9422bb30ee9ef10bf222761fe9e04442e3a11e88880c646487026011dae03dc281bc21c7d7ede5c2226d197befb813a4ecad686b559e5800");
        const auto recovered = PublicKey::recover(signature, message);
        EXPECT_EQ(hex(recovered.bytes), hex(publicKey.bytes));
    }
    { // same with v=27
        const auto message = parse_hex("6468eb103d51c9a683b51818fdb73390151c9973831d2cfb4e9587ad54273155");
        const auto signature = parse_hex("92c336138f7d0231fe9422bb30ee9ef10bf222761fe9e04442e3a11e88880c646487026011dae03dc281bc21c7d7ede5c2226d197befb813a4ecad686b559e581b");
        const auto recovered = PublicKey::recover(signature, message);
        EXPECT_EQ(hex(recovered.bytes), hex(publicKey.bytes));
    }
    { // same with v=35+2
        const auto message = parse_hex("6468eb103d51c9a683b51818fdb73390151c9973831d2cfb4e9587ad54273155");
        const auto signature = parse_hex("92c336138f7d0231fe9422bb30ee9ef10bf222761fe9e04442e3a11e88880c646487026011dae03dc281bc21c7d7ede5c2226d197befb813a4ecad686b559e5825");
        const auto recovered = PublicKey::recover(signature, message);
        EXPECT_EQ(hex(recovered.bytes), hex(publicKey.bytes));
    }
}

TEST(PublicKeyTests, isValidED25519) {
    EXPECT_TRUE(PublicKey::isValid(parse_hex("beff0e5d6f6e6e6d573d3044f3e2bfb353400375dc281da3337468d4aa527908"), TWPublicKeyTypeED25519));
    EXPECT_TRUE(PublicKey(parse_hex("beff0e5d6f6e6e6d573d3044f3e2bfb353400375dc281da3337468d4aa527908"), TWPublicKeyTypeED25519).isValidED25519());
    EXPECT_TRUE(PublicKey::isValid(parse_hex("fc8c425a8a94a55ce42f2c24b2fb2ef5ab4a69142d2d97f6c11e0106c84136d5"), TWPublicKeyTypeED25519));
    EXPECT_TRUE(PublicKey(parse_hex("fc8c425a8a94a55ce42f2c24b2fb2ef5ab4a69142d2d97f6c11e0106c84136d5"), TWPublicKeyTypeED25519).isValidED25519());
    EXPECT_TRUE(PublicKey::isValid(parse_hex("01beff0e5d6f6e6e6d573d3044f3e2bfb353400375dc281da3337468d4aa527908"), TWPublicKeyTypeED25519));
    EXPECT_TRUE(PublicKey(parse_hex("01beff0e5d6f6e6e6d573d3044f3e2bfb353400375dc281da3337468d4aa527908"), TWPublicKeyTypeED25519).isValidED25519());
    // Following 32 bytes are not valid public keys (not on the curve)
    EXPECT_TRUE(PublicKey::isValid(parse_hex("8eaf04151687736326c9fea17e25fc5287613693c912909cb226aa4794f26a48"), TWPublicKeyTypeED25519));
    EXPECT_FALSE(PublicKey(parse_hex("8eaf04151687736326c9fea17e25fc5287613693c912909cb226aa4794f26a48"), TWPublicKeyTypeED25519).isValidED25519());
    EXPECT_TRUE(PublicKey::isValid(parse_hex("51fdd5feae59d7dcbf5ebea99c05593ebee302577a5486ceac706ed568aa1e0e"), TWPublicKeyTypeED25519));
    EXPECT_FALSE(PublicKey(parse_hex("51fdd5feae59d7dcbf5ebea99c05593ebee302577a5486ceac706ed568aa1e0e"), TWPublicKeyTypeED25519).isValidED25519());
    // invalid input size/format
    EXPECT_FALSE(PublicKey::isValid(parse_hex("1234"), TWPublicKeyTypeED25519));
    EXPECT_FALSE(PublicKey::isValid(parse_hex("beff0e5d6f6e6e6d573d3044f3e2bfb353400375dc281da3337468d4aa5279"), TWPublicKeyTypeED25519));
    EXPECT_FALSE(PublicKey::isValid(parse_hex("02beff0e5d6f6e6e6d573d3044f3e2bfb353400375dc281da3337468d4aa527908"), TWPublicKeyTypeED25519));
    EXPECT_FALSE(PublicKey::isValid(parse_hex("0101beff0e5d6f6e6e6d573d3044f3e2bfb353400375dc281da3337468d4aa527908"), TWPublicKeyTypeED25519));
    EXPECT_FALSE(PublicKey(parse_hex("0399c6f51ad6f98c9c583f8e92bb7758ab2ca9a04110c0a1126ec43e5453d196c1"), TWPublicKeyTypeSECP256k1).isValidED25519());
}
