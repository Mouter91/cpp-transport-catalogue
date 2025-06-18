# 🚍 Transport Catalogue

C++‑проект для построения, сериализации и обработки транспортного справочника. Поддерживает генерацию SVG‑карты маршрутов и поиск оптимальных путей с учётом настроек маршрутизации.

---

## 📌 Возможности

- 📥 Загрузка маршрутов и остановок из JSON (`make_base`)
- 🗺 Построение SVG‑карты маршрутов (`Map`)
- 🧭 Поиск кратчайших маршрутов (`Route`)
- 📤 Обработка пользовательских запросов (`process_requests`)
- 💾 Сериализация и десериализация с помощью Protobuf

---

## 📁 Структура проекта

```
transport-catalogue/
├── main.cpp                     # Точка входа
├── transport_catalogue/
│   ├── domain.*                # Определения остановок и автобусов
│   ├── transport_catalogue.*  # Хранение и логика каталога
│   ├── map_renderer.*         # Отрисовка карты
│   ├── svg.*                  # SVG‑поддержка
│   ├── graph.*                # Структура графа
│   ├── router.*               # Алгоритм маршрутизации
│   ├── serialization.*       # Протокол сериализации (Protobuf)
│   ├── json_reader.*         # Чтение JSON-запросов
│   ├── json_builder.*        # Сборка JSON-ответов
│   └── log_duration.*        # Профилирование
```

---

## ⚙️ Сборка

### 🔧 Требования

- C++17 компилятор
- CMake ≥ 3.10
- Google Protobuf

### 🛠 Пример сборки

```bash
git clone https://github.com/Mouter91/cpp-transport-catalogue.git
cd cpp-transport-catalogue/transport-catalogue

mkdir build && cd build
cmake ..
cmake --build .
```

---

## 🚦 Использование

### ▶️ Команда `make_base`

Создаёт сериализованную базу данных из входного JSON:

```bash
./transport_catalogue make_base < base_input.json
```

### ▶️ Команда `process_requests`

Обрабатывает статистические запросы на основе сериализованной базы:

```bash
./transport_catalogue process_requests < stat_input.json > result.json
```

---

## 🧾 Формат JSON

### 📥 Пример `make_base` запроса

```json
{
  "base_requests": [ ... ],
  "render_settings": { ... },
  "routing_settings": {
    "bus_wait_time": 5,
    "bus_velocity": 40
  },
  "serialization_settings": {
    "file": "catalogue.db"
  }
}
```

### 📤 Пример `process_requests` запроса

```json
{
  "stat_requests": [
    { "id": 1, "type": "Bus", "name": "14" },
    { "id": 2, "type": "Map" }
  ],
  "serialization_settings": {
    "file": "catalogue.db"
  }
}
```

> Подходит как учебный проект по работе с географией, структурами данных, графами и визуализацией в C++.
