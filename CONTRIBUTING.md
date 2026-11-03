# Contributing Guidelines

Thank you for your interest in contributing to this project! We welcome contributions of all kinds, including bug reports, feature requests, documentation improvements, and code changes.

Please read this document before submitting your contribution.
----
## Ways to Contribute

*   **Reporting Bugs:** If you find a bug, please check the [Issues page](https://github.com/lvandeve/lodepng/issues) to see if it has already been reported. If not, open a new issue and provide clear steps to reproduce the bug.
*   **Suggesting Enhancements:** We are always looking for ways to improve. You can submit feature requests or suggestions on the [Issues page](https://github.com/lvandeve/lodepng/issues).
*   **Improving Documentation:** Typos, expanded explanations, or new tutorials are all valuable contributions.
*   **Writing Code:** Follow the guidelines below for code contributions.

----
## Code Contribution Workflow

We use the standard fork-and-pull-request workflow.

1.  **Fork the repository** to your own GitHub account.

2.  **Clone your fork** to your local machine:

    ```bash
    git clone https://github.com
    cd repository-name
    ```
3.  **Create a new branch** for your changes. Use a descriptive name that briefly explains what you are working on (e.g., `fix-typo-in-readme` or `add-user-authentication`):

    ```bash
    git checkout -b your-branch-name
    ```

4.  **Make your changes** in your text editor or IDE.

5.  **Commit your changes**. Use a concise, imperative commit message (e.g., `Add images to README`):

    ```bash
    git add .
    git commit -m "A concise description of your changes"
    ```

6.  **Push your changes** to your fork on GitHub:

    ```bash
    git push origin your-branch-name
    ```

7.  **Open a Pull Request** (PR) against the main repository. Go to your fork on GitHub and you should see a prompt to open a pull request. Fill out the template provided with a title and description of your changes.
----
## Code Standards

*   Ensure your code matches the project's existing coding standards, for new added codes (changes are not maded to the existing code) you must folow the coding style mentioned here:
    * For functions naming, use *Pascale case* style.
    * Function parameters must be prefexed with a lower case **`p`** and alsso must be in pascale case style.
    * Local variables must follow a *Snake case* style.
    * constants must be prefexed with a lower case **`k`**.
    * New data types or structs must follow a *Pascal case* style.
    * Static variables or structures must be prefixed with a lower case **`s`**...
    * The following table constains all the coding style rules to follow:
    | &nbsp;        Type | Prefixe | &nbsp;             Style |
    |:------|:---------:|:-------|
    |Function names| Lib name |&nbsp;Pascale case|
    |Function parameters| **p** | &nbsp;Pascale case (prefix excluded) |
    |Global variables| **g** | &nbsp;Pascale case |
    |Local variables |   | &nbsp;Snake case |
    |Static variables | **s** |   |
    |Constants | **k** |   |
    |Structures and types | **d** |&nbsp;Pascale case |
    
*   Test your changes thoroughly.
*   By contributing, you agree to the project rules and license.
----
**```Thank you for helping make this project better!```**
