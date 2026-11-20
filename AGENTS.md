# Agent Guidelines & Workflow Rules

These instructions define the mandatory operational protocols, theory note generation, and archiving procedures for AI agents working in this repository.

---

## 1. Automatic Chat Archiving Protocol

### Target Archive Directory & Serial Session Folders
All conversation sessions must be archived in dedicated serial folders inside `Prompts/` (e.g., `d:\GitHub\UITS\Computer_Graphics_Lab\Prompts\<N>. <Session Title>\`):
- **Folder Naming Convention**: `Prompts/<N>. <Session Title>/` (e.g., `Prompts/5. Shaders and Lighting Pipeline/`).
- **Determining Prefix $N$**: Before creating a new session folder, inspect `Prompts/` across both existing session folders and legacy flat `<N>. ...md` files, find the highest existing numeric prefix $N_{\text{max}}$, and assign $N = N_{\text{max}} + 1$.
- **Session Continuity**: Always update **the same session folder** on each turn or when resuming an existing conversation; never create a new major folder $N+1$ per turn.

### Required Files Inside `Prompts/<N>. <Session Title>/`
1. **`transcript_full.jsonl`**
   - An exact, unmodified UTF-8 copy of `<appDataDir>/brain/<conversation-id>/.system_generated/logs/transcript_full.jsonl`.
   - Never truncate, edit, or reformat lines in `transcript_full.jsonl`.
2. **`conversation.md` (Full Authentic Chat Archive)**
   - A complete Markdown rendering that **looks exactly the same as the AI response in the chat interface, alongside the user prompts**.
   - Renders each turn sequentially under `## Turn <N>` with `### 👤 User` (full verbatim prompt), `### 🧠 AI Thinking` (collapsible reasoning `<details><summary>Click to expand internal reasoning</summary>...</details>`), and `### 🤖 Assistant Response` (exact full response with all markdown headings, explanations, code snippets, OpenGL/shader details, and output verbatim).
   - **No Synthetic Executive Summaries**: Do NOT replace or truncate the chat archive with high-level summaries (such as the legacy `summary.md` format) that strip away the actual code, shader analysis, and explanations provided in the chat.
3. **Companion Plans & Walkthroughs (Optional, Fractional Suffixes)**
   - If plans or walkthroughs are generated in the session, place them inside the same folder as `<N>.1 Implementation Plan - <Plan Title>.md` and `<N>.2 Walkthrough - <Title>.md`.

### Update Cadence (Keeping the Archive Current)
Because the runtime writes a turn's final `PLANNER_RESPONSE` (including its final `thinking` and `content`) to `transcript_full.jsonl` after the turn finishes:
1. **At the start of each turn (or when continuing from a prior `<conversation-id>`):**
   - Copy `<appDataDir>/brain/<conversation-id>/.system_generated/logs/transcript_full.jsonl` to `Prompts/<N>. <Session Title>/transcript_full.jsonl` and regenerate `conversation.md` so the previous turn's finalized `thinking` and `content` are captured verbatim (and sync the prior conversation's folder if continuing).
2. **Before sending each new final response:**
   - Re-copy `transcript_full.jsonl` and regenerate `conversation.md` to capture all completed steps of the current turn (new user prompt, tool calls, and intermediate `thinking`).
   - Append the exact final response text being sent under `### 🤖 Assistant Response (Pre-Commit)` at the bottom of `conversation.md` (with a note that it will be replaced verbatim by the runtime-committed `thinking` + `content` from `transcript_full.jsonl` on the next sync).

---

## 2. Theory Note Generation & Documentation (`Lab_Codes/Learning/`)

### Target Directory & Naming
* **Directory**: `Lab_Codes/Learning/`
* **Naming Convention**: `<filename>-theory.md` (e.g. `3.RotateTriangle_mod-theory.md`).

### Mandatory Standards
1. **Clickable Table of Contents (TOC)**:
   - Every theory file must begin with a clickable Markdown Table of Contents right after the introduction/header block.
2. **GitHub Repository Link Prefix**:
   - Source code links must use absolute GitHub repository URLs in the format:
     `https://github.com/b1tranger/Computer_Graphics_Lab/blob/main/<path-from-repo-root>`
3. **Structured Explanations**:
   - Architectural and GPU pipeline concepts (VAO, VBO, Shaders, Uniforms).
   - Comparison tables, code trace, and common pitfalls/traps.

---

## 3. Full Authentic Chat Archive Format (`conversation.md`)

The conversation archive `Prompts/<N>. <Session Title>/conversation.md` must follow this structure, rendering every turn sequentially:

```markdown
# <N>. <Session Title>

- **Conversation ID**: `<conversation-id>`
- **Date**: `<YYYY-MM-DD>`
- **Model**: `<Model Name>`
- **Target File(s)**: [`<filename>`](https://github.com/b1tranger/Computer_Graphics_Lab/blob/main/<path-from-repo-root>)

---

## Turn 1

### 👤 User

<USER_REQUEST>
<Exact verbatim user prompt text, including any context tags>
</USER_REQUEST>

### 🧠 AI Thinking

<details>
<summary>Click to expand internal reasoning</summary>

<Verbatim model chain-of-thought>

</details>

### 🤖 Assistant Response

<The exact, complete AI response as rendered in the chat interface, containing all markdown headings, explanations, comparative tables, code blocks, and diagrams verbatim>

---

## Turn 2

### 👤 User
...

### 🧠 AI Thinking
...

### 🤖 Assistant Response
...
```

---

## 4. Mandatory Inclusions

1. **Full Verbatim `transcript_full.jsonl` & `conversation.md`**: Never omit or truncate user inputs, `PLANNER_RESPONSE.thinking`, or `PLANNER_RESPONSE.content` in `conversation.md`.
2. **GitHub File Links**: Reference inspected, created, or modified files using `https://github.com/b1tranger/Computer_Graphics_Lab/blob/main/<path-from-repo-root>`.
3. **Pre-Commit Pending Response**: Before sending each final response, record it verbatim under `### 🤖 Assistant Response (Pre-Commit)` in `conversation.md` so it is never missing prior to runtime commit.
