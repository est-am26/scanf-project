# scanf-project
Final project: implementation of scanf() in 

A robust and highly efficient custom implementation of the standard C `scanf` function. This project recreates standard input scanning behavior while introducing custom data types and advanced validation logic, ensuring a reliable alternative for complex input parsing.

## Features

- **Standard Specifiers:** Full support for `%d`, `%x`, `%f`, `%c`, and `%s`.
- **Custom Extensions:**
  - `%b`: **Binary** - Converts binary strings (e.g., `1010`) directly into integers.
  - `%L`: **Line** - Reads a full line until `\n` with intelligent whitespace handling.
  - `%D`: **Date** - Parses `DD/MM/YYYY` with strict leap year and calendar validation.
  - `%R`: **Color** - Parses hex color strings (`#RRGGBB`) into RGB structures.
- **Advanced Parsing:** - Supports **Field Width** (e.g., `%5d` to limit input length).
  - Supports **Assignment Suppression** (e.g., `%*s` to skip input).
  - Supports **Length Modifiers** (`h`, `hh`, `l`, `ll`).

## Supported Specifiers Reference

| Specifier | Description | Input Example |
| :--- | :--- | :--- |
| `%d` | Signed decimal integer (supports `h` to `ll`) | `42`, `-100` |
| `%x` | Unsigned hexadecimal integer (optional `0x`) | `0x1A`, `FF` |
| `%f` | Floating-point (decimal and scientific) | `3.14`, `1.2e-3` |
| `%b` | **Binary:** Reads 0s and 1s into an integer | `101010` |
| `%c` | Character(s): Reads width without null-term | `A`, `Hello` |
| `%s` | String: Reads until whitespace + adds `\0` | `Word` |
| `%L` | **Line:** Reads until a newline character | `Full sentence` |
| `%D` | **Date:** Validates calendar logic/leap years | `29/02/2024` |
| `%R` | **Color:** Parses hex into RGB components | `#FF5733` |

## Implementation Details

The project follows a **Modular "Helper-Core" Architecture**:
1. **Low-Level Helpers:** Functions like `skip_whitespace()` and `ungetc` management ensure the input stream remains consistent.
2. **Type-Specific Engines:** Specialized functions (e.g., `read_int`, `read_date`) handle the unique logic for each data type.
3. **Format Parser:** A central loop that interprets the format string, manages width/modifiers, and coordinates the data flow.

## Test Results

This implementation has been verified against a rigorous test suite covering edge cases, buffer overflows, and format mismatches.
- **Total Tests:** 342
- **Result:** 100% Pass  (All Clear)

## Getting Started

1. **Clone the repository:**
   ```bash
   git clone [https://github.com/est-am26/scanf-project.git]://github.com/YOUR_USERNAME/project-name.git)
2. **Compile:**
   ```bash
   gcc main.c my_scanf.c -o my_scanfcanf_scanf
3. **Run Tests:**
   ```bash
   ./my_scanf
