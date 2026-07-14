# AGENTS.md

## Project Overview

MkDocs documentation site for **010113330 Embedded System Laboratory for Telecommunications** (KMUTNB). 8 lab worksheets covering ESP32, MQTT, Raspberry Pi, Edge AI, and n8n workflow automation.

- **Primary language:** Thai (technical terms in English)
- **Deployed to:** GitHub Pages (`pruks-dev.github.io/embedded-telecom`)
- **Theme:** Material for MkDocs (deep purple)

---

## Build / Serve / Deploy

```bash
# Install dependencies
pip install -r requirements.txt

# Local dev server (auto-reload on changes)
mkdocs serve
# → http://127.0.0.1:8000/embedded-telecom/

# Build static site
mkdocs build --site-dir site

# If encrypted content cache is stale (e.g. after removing `level: staff`)
rm encryptcontent.cache && mkdocs serve
```

### CI/CD

Push to `main` triggers `.github/workflows/deploy.yml`:
1. Checkout → Python 3.10 → `pip install`
2. `mkdocs build --site-dir site`
3. Deploy to GitHub Pages via `upload-pages-artifact@v4` + `deploy-pages@v5`

No PR previews — deploy is production-only on `main`.

---

## Project Structure

```
docs/                          # All source content
  index.md                     # Homepage, course overview
  setup.md                     # Software installation guide (Arduino IDE, ESP32)
  ใบงานการทดลองที่ 1.md        # Lab 1: Sensor Node
  ใบงานการทดลองที่ 2.md        # Lab 2: Data Communication (TCP/HTTP/MQTT)
  ใบงานการทดลองที่ 3.md - 8.md # Labs 3-8
  images/                      # Rendered diagrams and photos (PNG/JPG)
    lab-1/
    lab-2/ ...
  src/                         # Source code for each lab (Arduino sketches, Python)
    lab-1/
    lab-2/ ...

mkdocs.yml                     # MkDocs configuration
requirements.txt               # Python dependencies
.github/workflows/deploy.yml   # GitHub Actions deploy pipeline
```

---

## Markdown Conventions

### Section structure

```markdown
# ใบงานการทดลองที่ N: Title

---
## วัตถุประสงค์
- Objective bullets...

## อุปกรณ์ที่ใช้ในการทดลอง
| ลำดับ | อุปกรณ์ | จำนวน |
|---|---|---|

---
# การทดลองที่ N.1 Title

## ขั้นตอนการทดลอง
1. Step...

## บันทึกผลการทดลอง
| Column | ... |
|---|---|

---
# สรุปผลการทดลอง

---
# คำถามท้ายใบงาน
1. ...
```

- Use `---` as a horizontal rule separator between major sections.
- Use `# Heading 1` for experiment titles, `## Heading 2` for subsections.
- Every experiment ends with a **บันทึกผลการทดลอง** table for students to fill in.
- Final questions go under **คำถามท้ายใบงาน** — no per-experiment sub-questions (avoid duplication).

### Code blocks

```markdown
   ```cpp
   // Arduino/ESP32 code — indent 3 spaces inside list items
   ```

   ```python
   # Python code
   ```

   ```bash
   # Shell commands
   ```

   ```mermaid
   graph LR    # or sequenceDiagram
   ```
```

- C++ code uses `#include` with angle brackets, `const char*`, camelCase for vars.
- Python code is Python 3, uses stdlib where possible (no external deps unless necessary).

### Tables

Use `|---|` separators. Column alignment via `:---` (left), `:---:` (center), `---:` (right):

```markdown
| ลำดับ | อุปกรณ์ | จำนวน |
| --- | --- | --- |
```

### Admonitions (callouts)

```markdown
> 💡 Informational note
> ⚠️ Warning / caveat
```

---

## Diagrams (Mermaid)

Enabled via `pymdownx.superfences` in `mkdocs.yml`. Use in any `.md` file:

````markdown
```mermaid
graph LR
    A[Node] -->|label| B[Node]

sequenceDiagram
    A->>B: message
```
````

Mermaid JS loaded from CDN in `mkdocs.yml`. No plugin required.

---

## Encrypted Content

`mkdocs-encryptcontent-plugin` protects pages with `level: staff` frontmatter:

```yaml
---
level: staff
---
```

To remove protection, delete the frontmatter block AND clear the cache:

```bash
rm encryptcontent.cache
```

---

## Commit / PR Conventions

- **Prefix commits** with category: `lab N:`, `ci:`, `fix:`, `docs:`
- **Push directly to `main`** (no PR required for single-author repo)
- Use Thai for lab content, English for CI/config commits
- Never commit: `site/`, `encryptcontent.cache`, `.DS_Store`, `.env` (covered by `.gitignore`)

---

## Naming Conventions

| Context | Convention | Example |
|---|---|---|
| Lab files | `ใบงานการทดลองที่ N.md` | `ใบงานการทดลองที่ 2.md` |
| Images | `docs/images/lab-N/descriptive-name.png` | `images/lab-2/mqtt-arch.png` |
| Source code | `docs/src/lab-N/filename.ino` or `.py` | `src/lab-2/receiver.py` |
| MQTT topic | `kmutnb/<student_id>/telemetry` | `kmutnb/66010001/telemetry` |
| ESP32 client ID | `site_<student_id>` | `site_66010001` |

---

## Embedded Code Conventions

When writing ESP32/Arduino code in markdown:

- Use `const char*` for strings, `const int` for constants.
- Prefer `StaticJsonDocument<N>` over `DynamicJsonDocument` for ESP32.
- Include `// TODO: เปลี่ยนเป็น...` comments where students must personalize (e.g. WiFi SSID, client ID).
- Show full, runnable snippets — not pseudocode fragments.
- Non-blocking timing via `millis()` for heartbeat/interval tasks — never `delay()` in production but `delay()` is acceptable for lab simplicity.
- Use `Serial.printf()` for formatted debug output.

Python snippets for the course:
- Use stdlib (`http.server`) not Flask/FastAPI (zero install).
- Single-file, copy-paste runnable.

---

## `mkdocs.yml` Key Config

- Python 3.10 on CI, `mkdocs>=1.6,<2.0`, `mkdocs-material>=9.5,<10.0`
- Mermaid via `pymdownx.superfences.custom_fences`
- MathJax for LaTeX math (loaded from CDN)
- Encrypt content plugin: password via `STAFF_PASSWORD` env var
- Thai language: `language: th`
