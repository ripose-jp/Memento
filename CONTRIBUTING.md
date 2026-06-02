# Contributing Guidelines

## Code Style

### C and C++

* Maintain an 80 character line width at all times.

  The only exceptions to this are in cases where lines longer than 80 characters increase readability.
  Examples of this are inline HTML or SVG markup, or code that makes heavy use of databases such as in `yomidbbuilder.c`.
* Use Allman braces:
  ```c
  long foo(int var)
  {
      if (var < 0)
      {
          return 0;
      }

      long i = 1;
      while (var < 20)
      {
          i *= var;
          ++var;
      }

      return i;
  }
  ```

  Examples of incorrect bracing:
  ```c
  long foo(int var) {
      ...
  }
  ```

  ```c
  long foo(int var)
  {
      if (var < 0) {
          ...
      }
  }
  ```
* `if`-statements and `while`-statements must always have braces even if they only surround one line.

  ```c
  if (var < 0)
  {
      return;
  }
  ```

  Examples of incorrect bracing:

  ```c
  if (var < 0) return;
  ```

  ```c
  if (var > 0)
  {
      ++var;
      foo *= var;
  }
  else
      return var;
  ```
* Use 4 spaces for indentation.
* Remove all trailing whitespace.
* When calling a function that will take more than 80 characters, put arguments on a new line and the closing `)` on its own line:
  ```cpp
  reallyLongObjectName->doSomething(
      REALLY_REALLY_LONG_DEFINE_NAME,
      AN_EVEN_LONGER_CONSTEXPR_CONSTANT_NAME,
      nullptr
  );
  ```
* For magic numbers or string constants, using `constexpr` variables is preferred.
  Use your best judgement for this.
* For conditional or arithmetic statements spanning multiple lines, operators go at the end of the line.

  Example:
  ```c
  if (cond &&
      cond2 &&
      cond3)
  {
      ...
  }
  ```

* Includes are ordered alphabetically with new lines separating each section.
  Include sections are ordered as follows:
  * Header file/superclass
  * stdlib includes
  * Qt includes
  * Other library includes
  * Project-wide includes (e.g. `foo.h` is in `src/foobar/foo.h` and is included
    in `car.c` which is in `src/barcar/car.c`)
  * Forward declarations
* For C++, header files use `.h` and code uses `.cpp`.
  For C, headers use `.h` and code uses `.c`.
* Every function contains a function header that describes what it does, what
  parameters it takes, and what it returns.
  This header will be placed above the first function declaration.
* Every class contains a class header describing the purpose of the class.
* Every member/instance variable contains a comment describing its function.
* All member/instance variables start with `m_`.
  Variables inside of a member struct do not need to be prefixed with `m_`.
* C++ will generally use `camelCase` while C will generally use `snake_case`.
* In C/C++ implementation files, contents are ordered as:
    * Copyright Header
    * Includes
    * Defines
    * Constructor
    * Destructor
    * Initializers
    * All other functions
* All functions in an implementation file should be grouped with other related functions with comments denoting the start and end of each section.
  These comments are of the form `/* Begin <Section> */` and `/* End <Section> */`.
* Comments on their own line should use `/* */` while inline comments should use `//`.

### QML

* Generally try to follow [QML coding conventions](https://doc.qt.io/qt-6/qml-codingconventions.html).
* Order your imports alphabetically.
* Use Allman braces for JavaScript code except in the `function` declaration line.
  Put the brace on the same line as the `function` keyword.

  Example:
  ```JavaScript
  function foo(bar) {
      if (bar)
      {
          return true;
      }
      return false;
  }
  ```
* Add header comments to your functions describing their purpose, what arguments they take, and what they return.

## Commits

* All commits must have a message line that follows these rules:
  * The message line is all lowercase.
  * Starts with the file/feature the changes primarily apply to.
  * Summarize the changes made.

* Commits more complex than fixing typos must have a message body that describes the changes made.
* The message body is separated from the commit message by a single new line.
* Commit messages can be no wider than 72 characters.

  An example of a good commit message is:
  ```
  subtitlewidget: fixes bug where subtitles would fail to show up

  Subtitles would fail to show up because the startTime variable stores
  the time as a double. Because doubles are imprecise, it would trigger
  the if-statement for clearing the subtitle in the durationChanged
  method. This fixes the problem by adding a delta value to account for
  double imprecision.
  ```

* Use as few commits as possible when implementing a feature.
  PRs containing multiple commits should generally implement multiple features.
* When fixing up commits, please autosquash and force push changes to keep things clean.
  See this [link](https://stackoverflow.com/questions/3103589/how-can-i-easily-fixup-a-past-commit) for how to do so.

## Languages

* Frontend code should be in QML and C++.
* Backend code can be in either C++ or C.
* Other languages like Objective-C can be used when necessary.

## Unacceptable Changes

* Changing default mpv binds.
  For example binding `sub-seek 1` to the right arrow key.
* Importing QtWebEngine.
* Adding support for languages other than Japanese.
  If you would like to do this, feel free to start your own fork.
* Adding features that depend non-portable APIs when they can be implemented
  using portable APIs.
* Using GPLv2 incompatible code.
* Changes that infringe on copyright.

## Legal

* All contributions are assumed to be licensed under GPLv2 only (only as in not GPLv2 or later).
* Do not add your name to the copyright header unless you created the file.
* Make sure the copyright header contains the year the file was created.

## AI Policy

AI code and AI-assisted code is accepted so long as you understand the code you are contributing and the changes are good.
You must disclose the model or software used to generate the code.
Disclosures are only necessary if AI made substantial contributions to the code.
For example, using AI to review your code doesn't need to be disclosed.
Generally the less complex the code, the more likely it is to be accepted.
