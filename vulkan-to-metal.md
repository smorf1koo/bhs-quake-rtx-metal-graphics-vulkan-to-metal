# Vulkan → Metal: карта соответствий

> Шаблон к тестовому заданию «Graphics Engineer», Bear Head Studio · Quake RTX на Apple Metal.
> Скопируйте файл в корень своего репозитория и заполните.
> **Обязательна только часть А.** Часть Б относится к дополнительной части задания, её можно не заполнять.
> Ответы короткие: 1–2 строки в ячейке, своими словами. Ссылки на документацию приветствуются.

**Автор:** …
**Трек:** Mac (Metal) / без Mac (Vulkan) — нужное оставить

---

## Часть А (обязательно). 8 базовых понятий

Колонки:
- **Vulkan** — объект или вызов.
- **Metal** — ближайший аналог. Если прямого аналога нет, так и напишите.
- **Главное отличие** — одна фраза: кто за что отвечает, когда это происходит.

| # | Понятие | Vulkan | Metal | Главное отличие |
|---|---|---|---|---|
| 1 | Очередь и командный буфер | `VkQueue`, `VkCommandPool`, `VkCommandBuffer` | | |
| 2 | Отправка работы на GPU | `vkQueueSubmit` | | |
| 3 | Проход рендеринга | `VkRenderPass` / dynamic rendering (`vkCmdBeginRendering`) | | |
| 4 | Графический пайплайн | `VkPipeline`, `VkPipelineLayout` | | |
| 5 | Привязка ресурсов к шейдеру | descriptor set layout / descriptor sets | | |
| 6 | Синхронизация CPU ↔ GPU | `VkFence`, `vkWaitForFences` | | |
| 7 | Барьеры и layout изображений | `vkCmdPipelineBarrier`, image layouts | | |
| 8 | Вывод на экран | `VkSwapchainKHR`, acquire / present | | |

### Синхронизация в моём приложении (обязательно, 5–10 предложений)

Ответьте на три вопроса про свой код:

- Сколько кадров «в полёте» и почему.
- Где CPU ждёт GPU и где GPU сообщает CPU, что кадр готов.
- Что сломается, если убрать это ожидание: какие данные перезапишет CPU и когда.

…

---

## Часть Б (дополнительно). Расширенная таблица и Q2RTX

Добавьте строки и колонку **«Сложнее всего при переносе Q2RTX»**: что конкретно вызовет проблемы в `src/refresh/vkpt/` и как бы вы это решали. Строки-подсказки (можно менять):

| Понятие | Vulkan | Metal | Главное отличие | Сложнее всего при переносе Q2RTX |
|---|---|---|---|---|
| Семейства очередей, несколько очередей | | | | |
| Вторичные command buffers, параллельная запись | | | | |
| Compute pipeline и dispatch | | | | |
| Шейдеры: SPIR-V ↔ MSL, кросс-компиляция | | | | |
| Pipeline cache, предкомпиляция шейдеров | | | | |
| Push constants | | | | |
| Specialization constants | | | | |
| Bindless, descriptor indexing (`GL_EXT_nonuniform_qualifier`) | | | | |
| Выделение памяти (memory types, VMA) ↔ storage modes, `MTLHeap` | | | | |
| Семафоры (binary / timeline), events | | | | |
| Hazard tracking: кто отслеживает зависимости | | | | |
| Ресайз окна, out-of-date swapchain | | | | |
| Структуры ускорения (BLAS / TLAS), RT pipeline | | | | |
| Timestamp queries | | | | |
| Validation layers / RenderDoc ↔ Metal API Validation / Xcode GPU Capture | | | | |

### Топ-3 риска переноса Q2RTX на Metal

1. …
2. …
3. …

## Источники

- …
