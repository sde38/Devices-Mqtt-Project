#pragma once
#include <concepts>
#include <QObject>

// 1. On vérifie que le type T possède un membre .id convertible en int
template <typename T>
concept HasIntegralId = requires(T a) {
    { a.id } -> std::convertible_to<int>;
};


// 2. On vérifie qu'un type U dérive bien de QObject
template <typename U>
concept QObjectDerived = std::derived_from<U, QObject>;