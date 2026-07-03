// Ici, on teste la structure de base et on valide qu'elle répond bien au concept C++20 HasIntegralId.

#include <gtest/gtest.h>
#include <Structures.h>
#include <Concept_1.h>

// Test de la création et des valeurs par défaut
TEST(HardwareDeviceTest, DefaultConstructionAndProperties) {
    Device device{42, "Test GPU", DeviceType::GPU, 55.0f, 10.0f};

    EXPECT_EQ(device.id, 42);
    EXPECT_EQ(device.nom, "Test GPU");
    EXPECT_EQ(device.type, DeviceType::GPU);
    EXPECT_FLOAT_EQ(device.temperature, 55.0f);
    EXPECT_FLOAT_EQ(device.charge, 10.0f);
}

// Test crucial : Est-ce que notre structure valide bien le concept C++20 ?
TEST(HardwareDeviceTest, CompliesWithConcepts) {
    // Si le concept n'est pas validé, cette ligne provoquera une erreur de compilation volontaire
    static_assert(HasIntegralId<Device>, "HardwareDevice doit valider le concept HasIntegralId");
    SUCCEED(); // Si ça compile, le test est réussi
}