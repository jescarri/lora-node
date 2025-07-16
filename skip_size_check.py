Import("env")

# Skip the program size check as the partition table dictates the size allowed

def skip_size_check(source, target, env):
    print("Skipping program size check due to custom partition table")
    return None

# Try to replace the size check action
env.Replace(PROGSIZE_ACTION=skip_size_check)

