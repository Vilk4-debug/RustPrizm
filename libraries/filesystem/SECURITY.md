# Security Policy

## Supported Versions

Security fixes are provided for the latest published release of
ghc::filesystem. Older releases are not normally maintained.

The currently supported release is v1.5.16.

Users should update to the latest release before reporting a vulnerability
that might already have been fixed.

## Reporting a Vulnerability

Please do not open a public GitHub issue for a suspected security
vulnerability.

Use the **Report a vulnerability** button in the repository's
[Security Advisories](https://github.com/gulrak/filesystem/security/advisories)
section.

If private vulnerability reporting is unavailable, contact
s.schuemann@pobox.com with the subject "ghc::filesystem security report".

Please include, where possible:

- The affected ghc::filesystem version or commit.
- The operating system, compiler, architecture, and language standard.
- A description of the security impact and expected attack scenario.
- Reproduction steps or a minimal proof of concept.
- Whether untrusted input is required to trigger the problem.
- Any known workarounds or suggested fixes.
- Any planned public disclosure date, and whether and how you would like to be credited.

Reports should contain enough information to distinguish a security
vulnerability from an ordinary correctness or standards-conformance issue.

## Scope

Examples of potentially security-relevant issues include:

- Memory corruption or undefined behavior reachable through untrusted input.
- Path parsing or normalization errors that cause an operation to affect a
  different filesystem location than specified by the API.
- Symlink, reparse-point, or namespace handling that bypasses documented
  behavior.
- Encoding or argument-conversion bugs that cause memory-safety problems,
  unintended filesystem operations, or practical denial of service.
- Error-handling defects that result in invalid memory access or silently
  performing an unsafe operation.

Ordinary conformance differences, unsupported-platform problems, and
application-level path traversal caused solely by passing unsanitized paths to
filesystem operations should be reported as regular GitHub issues.

ghc::filesystem provides filesystem primitives and does not provide sandboxing,
access control, or validation of application-specific path policies.

## Preferred Languages

I prefer all communications to be in English, but German works too.

## Response and Disclosure

I will try to acknowledge reports within seven days. Investigation and release
timelines depend on the affected platforms and the complexity of reproducing
the issue.

Please allow time to investigate and prepare a fix before public disclosure.
When appropriate, the fix and disclosure will be coordinated through a GitHub
Security Advisory. Reporters will be credited unless they prefer to remain
anonymous.

This project does not operate a paid bug bounty program and can not afford one.
