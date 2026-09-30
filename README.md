# 🖥️ Server Monitor

Лёгкая система мониторинга Linux-сервера на **C++20**. Приложение в фоновом потоке собирает метрики системы, сохраняет их в PostgreSQL, отслеживает превышение порогов и отдаёт данные через встроенный HTTP-сервер — вместе с веб-дашбордом.

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue) ![PostgreSQL](https://img.shields.io/badge/PostgreSQL-16-336791) ![Docker](https://img.shields.io/badge/Docker-ready-2496ED) ![Linux](https://img.shields.io/badge/platform-Linux-lightgrey)

## Возможности

- **Сбор метрик** напрямую из ядра Linux, без сторонних агентов:
  - CPU — загрузка по `/proc/stat`
  - Память — `/proc/meminfo`
  - Диск — `statvfs`
  - Сеть — входящий/исходящий трафик по `/proc/net/dev`
  - Процессы — обход `/proc/<pid>`
- **Хранение истории** метрик в PostgreSQL (libpq)
- **Алерты** при превышении настраиваемых порогов CPU / RAM / диска и уведомление о возврате в норму
- **REST API** на Boost.Asio
- **Веб-дашборд** (HTML/CSS/JS) с текущими показателями, статусом сервера и БД, графиком истории, таблицей процессов
- **Конфигурация** через JSON-файл или переменную окружения `SERVER_MONITOR_CONFIG`
- Корректное завершение по `SIGINT`, логирование
- Сборка и запуск в **Docker** (multi-stage build + docker-compose)

## Стек

C++20 · Boost.Asio · PostgreSQL (libpq) · CMake · Docker / docker-compose · HTML / CSS / JavaScript

## Архитектура

```
             ┌──────────────┐   каждые N сек   ┌──────────────┐
  /proc ───▶ │  Collectors  │ ───────────────▶ │   Monitor    │ (фоновый поток)
  statvfs    │ CPU/RAM/Disk │                  └──────┬───────┘
             │ Net/Process  │                         │
             └──────────────┘            ┌────────────┼────────────┐
                                         ▼            ▼            ▼
                                   AlertManager   Database    HttpServer
                                    (пороги)    (PostgreSQL)  (Boost.Asio)
                                                                  │
                                                     REST API + веб-дашборд
```

```
src/
├── collector/   # сборщики метрик (CPU, память, диск, сеть, процессы)
├── monitor/     # цикл опроса в отдельном потоке
├── alert/       # проверка порогов и алерты
├── database/    # работа с PostgreSQL
├── server/      # HTTP-сервер и REST API
├── config/      # загрузка JSON-конфигурации
├── models/      # структуры метрик
├── utils/       # логгер
└── main.cpp
web/             # дашборд (index.html, style.css, script.js)
config/          # примеры конфигурации
docker/          # docker-compose
tests/           # тесты
```

## REST API

| Метод | Эндпоинт | Описание |
|---|---|---|
| GET | `/api/metrics` | Текущие метрики: CPU, память, диск, сеть, процессы |
| GET | `/api/status` | Статус сервера и подключения к БД |
| GET | `/api/history` | История метрик из PostgreSQL |
| GET | `/` | Веб-дашборд |

## Конфигурация

`config/config.example.json`:

```json
{
    "server_port": 8080,
    "monitor_interval": 5,
    "cpu_limit": 80,
    "memory_limit": 90,
    "disk_limit": 90,
    "database_host": "localhost",
    "database_port": 5432,
    "database_name": "server_monitor",
    "database_user": "monitor",
    "database_password": "CHANGE_ME"
}
```


