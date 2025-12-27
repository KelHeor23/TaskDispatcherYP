#pragma once

#include <array>
#include <memory>

namespace dispatcher {

enum class TaskPriority { High, Normal, Count };

// Для точной настройки последовательности
constexpr std::array<TaskPriority, static_cast<std::size_t>(TaskPriority::Count)> AllTaskPriorities = {
    TaskPriority::High, TaskPriority::Normal};

}  // namespace dispatcher