# Agent Guidelines & Workflow Rules

These instructions define the mandatory operational protocols, theory note generation, and archiving procedures for AI agents working in this repository.

---

## 1. Automatic Chat Archiving Protocol

### Target Archive Directory
All conversation sessions, including prompts, AI thinking processes, actions taken, and final responses, must be saved and archived in:
`Prompts/` (e.g. `d:\GitHub\UITS\Computer_Graphics_Lab\Prompts\`)

### Mandatory File Rules for `Prompts/`

1. **A New File For Each Chat Session**:
   - A brand new markdown archive file must be created in `Prompts/` for every separate chat session.
   - Never combine, merge, or overwrite archives from previous chat sessions.

2. **Mandatory Sequential Number Prefix**:
   - **Every** file created in `Prompts/` MUST start with an incrementing sequential numeric prefix followed by a period and a space:
     `N. <Descriptive Title>.md`
   - **Determining Prefix $N$**: Before creating a new archive file, inspect the `Prompts/` directory, find the highest existing numeric prefix $N_{\text{max}}$, and assign:
     $$N = N_{\text{max}} + 1$$
   - **Strict Enforcement**: No file may ever be added to `Prompts/` without its designated `N. ` prefix.

- **Examples of Existing & Future Files**:
  - `1. Creating Circular GLUT Graphics.md`
  - `2. Drawing Colored Borders and Practice Project.md`
  - `3. OpenGL Graphics Notes and Custom Agent Instructions.md`
  - `4. Explanation of Apple Forward Compatibility Hint in GLFW.md`
  - `5. <Next Chat Topic Title>.md`

### Frequency of Updates
- **On Each Modification**: Whenever any code, note, configuration, or file is created or modified in response to a user request, the active conversation archive in `Prompts/` must be updated or created in the same turn.
- **Multi-Turn Continuity within a Session**: When a single chat conversation spans multiple turns, append the subsequent user queries, thinking steps, actions, and responses to that chat's active session file so the complete history of that session remains intact.

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

## 3. Archive File Structure & Format

Each archive file in `Prompts/` must adhere to the following Markdown format:

```markdown
# Chat Conversation

Note: _This is an archived log of the conversation session including user inputs, AI thinking processes, tools executed, and final planner responses._

---

### User Input
[Exact user request or prompt verbatim]

### AI Thinking Process
> [Full step-by-step reasoning, architectural considerations, problem-solving analysis, and design decisions formulated by the AI prior to and during tool execution.]

### Actions Executed
- *[Tool action summary, e.g., Listed directory, Viewed file, Created file]*

### Planner Response
[Full final response delivered to the user]

---
```

---

## 4. Mandatory Inclusions

1. **AI Thinking Process**: Never omit or truncate the model's internal reasoning. The thought process explains why decisions were made, why particular OpenGL functions or GLSL constructs were selected, and how problems were decomposed.
2. **Tool Invocations & Files Modified**: Explicitly mention all inspected, created, or modified file paths with markdown links.
3. **Response Completeness**: Include the full final response (`### Planner Response` / `### AI Response`) verbatim exactly as delivered in the chat (never use placeholders like `*(Refer to output in main chat)*` or truncated summaries), including all generated code snippets, diagrams, tables, and explanations.
