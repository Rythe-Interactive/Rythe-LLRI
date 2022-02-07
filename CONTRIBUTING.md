## Repository management
### Git-Branch methodology
This repository employs a git flow based workflow, to incorporate your changes the following conditions must be true:
- Your change lives on either feature/your_change_name, task/your_change_name, or bugfix/your_change_name.
- You must create a pull request.
- If your branch is a bugfix it must mention an issue.
- It must adhere to the code-style mentioned below.

Furthermore the following rules are to be met when attempting to merge:
- To merge into develop, you require the approval of one peer reviewer and the ci must pass.
- To merge into main, you require the approval of one code-owner and the ci must pass.
- Hotfix branches are only allowed in special cases and must be communicated with a code owner.

## Code style

### File-Names
- A file name cannot have any uppercase letters
- Headers must have the ending .hpp or .h respectively. (use hpp for c++ headers)
- Source must have the ending .cpp or .c respectively. (use cpp for c++ files)

### Namespacing

Everything needs to go into the namespace `llri`, functions and classes that aren't part of the public API should go into `llri::detail`, no other namespaces are allowed. Use the c++17 convention for nested namespaces (llri::detail).

### Class/Struct Names

Types that provide a lot of functionality unlike "stl-likes" should be classes and start with an uppercase letter and continue in PascalCase.<br>
(examples: Instance, CommandList, Resource)

POD types, "stl-likes", and primitive typedefs should all be structs, and use stl-like snake_case naming.<br>
(examples: instance_desc, extent_2d, submit_desc\<float\>)

### Class/Struct Members
`private` variables should start with `m_` and continue in camelCase.<br>
`public` variables should not have a prefix and use camelCase, the same applies to local variables.

Public API methods should use camelCase naming, whereas private implementation methods should use camelCase naming prefixed with impl_ (e.g. impl_createInstance)

API Methods should follow the following conventions:
* `create...` takes a `..._desc`, and a pointer to a `...` output variable.
* `destroy...` only takes the `...` object to destroy.
* `query...` queries data using implementation API functions.
* `get...` gets data without using implementation API functions (e.g. a cached state or value).

### Comments
All public API methods and classes **must** be documented on their function and (valid/invalid) usage. Pull requests **must not** be accepted if documentation is insufficient. Check The Chapter "On the Topic of Doxygen" at the end of this document.

### Templates
When doing template-meta-programming make sure to use `constexpr` and `using` correctly.

### On the Topic of Doxygen
To ensure that your doxygen comments are in line with the rest of the code base we have created some basic rules you should follow:

- We exclusively use `@` and `/**` to document our files. for example:
  ```cpp
    /**
     * @brief Create an llri Instance, which can be used to enumerate adapters and create a few core objects.
     * Like with all API objects, the user is responsible for destroying the Instance again using destroyInstance().
     *
     * @param desc The description of the instance.
     * @param instance The instance to be created.
     *
     * @note instance **must**  be a valid pointer to an Instance* variable.
     *
     * @return Success upon correct execution of the operation.
     * @return Values if any of the conditions in instance_desc are not met: ErrorInvalidUsage, ErrorExtensionNotSupported.
     * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorInitializationFailed, ErrorIncompatibleDriver.
    */
    result createInstance(const instance_desc& desc, Instance** instance);
  ```

- To keep the file small we encourage you to drop any commands, when the function is small enough to be explained by `@brief`
alone. for example:
  ```cpp
  /** @brief Gets the size of the container.
   *  @return size_type The size of the container.
   */
  auto size() const noexcept
  {
    return m_container.size();
  }
  ```
  should instead be
  ```cpp
  /** @brief Gets the size of the container.
   */
  auto size() const noexcept
  {
    return m_container.size();
  }
  ```
  To decide if a type is simple enough to drop info about extra commands, consider how much text you would repeat & how complicated the types are you are taking or returning.

- Try to adhere to proper English. While this is not a super critical condition. we do appreciate good punctuation and spelling (we also make these mistakes at times). A good spell-check plugin for your favorite editor helps tremendously.

- Default Constructors & Destructors should not be documented, unless it does something very unexpected. In which case you should ask yourself if a constructor is the right place for your custom behavior
