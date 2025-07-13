# Tag Validation Quick Reference

## ✅ Valid Tags (Pipeline will succeed)

| Tag | Type | Description |
|-----|------|-------------|
| `1.0` | Float | Basic float number |
| `1.1` | Float | Float with decimal |
| `2.0` | Float | Integer with decimal |
| `10.2` | Float | Multi-digit float |
| `0.1` | Float | Zero major version |

## ❌ Invalid Tags (Pipeline will FAIL)

| Tag | Reason | Correct Format |
|-----|--------|----------------|
| `v1.0` | Starts with 'v' | `1.0` |
| `1.0.0` | Semantic versioning (3 parts) | `1.0` |
| `1.0.1` | Semantic versioning (3 parts) | `1.0` |
| `1` | No decimal point | `1.0` |
| `1.` | No digits after decimal | `1.0` |
| `version1.0` | Contains letters | `1.0` |
| `1.0a` | Contains letters | `1.0` |
| `1.0-beta` | Contains letters/hyphens | `1.0` |

## Quick Commands

### Create a Valid Release
```bash
# Create tag
git tag 1.0

# Push tag (triggers release pipeline)
git push origin 1.0
```

### Check Tag Format Before Pushing
```bash
# Test if tag is valid (bash)
TAG="1.0"
if [[ "$TAG" =~ ^[0-9]+\.[0-9]+$ ]] && [[ "$TAG" != v* ]]; then
    echo "✅ Valid tag: $TAG"
else
    echo "❌ Invalid tag: $TAG"
fi
```

### List All Tags
```bash
git tag --list
```

### Delete Invalid Tag (if pushed)
```bash
# Delete local tag
git tag -d v1.0

# Delete remote tag
git push origin --delete v1.0
```

## Pipeline Behavior

### Valid Tag Flow
1. ✅ Tag validation passes
2. ✅ Unit tests run
3. ✅ Compilation with firmware version
4. ✅ Artifacts created
5. ✅ GitHub release created

### Invalid Tag Flow
1. ❌ Tag validation fails
2. ❌ Pipeline stops immediately
3. ❌ No artifacts created
4. ❌ No release created

## Common Mistakes

1. **Using 'v' prefix** - GitHub convention but not allowed here
2. **Using semantic versioning** - Only X.Y format allowed (not X.Y.Z)
3. **Missing decimal point** - Must have exactly one decimal
4. **Trailing decimal** - Must have digits after decimal
5. **Using letters** - Only numbers and one dot allowed

## Float Number Format

Only simple float numbers are accepted:
- **Format**: `X.Y` where X and Y are numbers
- **Examples**: `1.0`, `1.1`, `2.0`, `10.2`
- **Not allowed**: `1.0.0`, `1.0.1`, `v1.0`

## Examples in Context

```bash
# Development workflow
git checkout -b feature/new-feature
# ... make changes ...
git push origin feature/new-feature
# Branch pipeline runs

# Merge to main
git checkout main
git merge feature/new-feature
git push origin main
# Main pipeline runs

# Create release (FLOAT NUMBERS ONLY)
git tag 1.0
git push origin 1.0
# Release pipeline runs and creates artifacts

# Next release
git tag 1.1
git push origin 1.1
# Release pipeline runs and creates artifacts
``` 