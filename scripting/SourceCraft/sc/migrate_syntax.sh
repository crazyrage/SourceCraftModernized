#!/bin/bash
# SourcePawn 1.13 Syntax Migration Script

# Function to migrate a single file
migrate_file() {
    local file="$1"
    echo "Migrating: $file"

    # Create backup
    cp "$file" "$file.bak"

    # Apply transformations using sed
    # 1. new bool: → bool
    sed -i 's/\bnew bool:/bool/g' "$file"

    # 2. new Float: → float
    sed -i 's/\bnew Float:/float/g' "$file"

    # 3. new String: → char
    sed -i 's/\bnew String:/char/g' "$file"

    # 4. decl String: → char
    sed -i 's/\bdecl String:/char/g' "$file"

    # 5. new Handle: → Handle
    sed -i 's/\bnew Handle:/Handle/g' "$file"

    # 6. stock bool: → stock bool
    sed -i 's/\bstock bool:/stock bool/g' "$file"

    # 7. stock Float: → stock float
    sed -i 's/\bstock Float:/stock float/g' "$file"

    # 8. stock static Float: → stock static float
    sed -i 's/\bstock static Float:/stock static float/g' "$file"

    # 9. stock static String: → stock static char
    sed -i 's/\bstock static String:/stock static char/g' "$file"

    # 10. stock static Handle: → stock static Handle
    sed -i 's/\bstock static Handle:/stock static Handle/g' "$file"

    # 11. stock static bool: → stock static bool
    sed -i 's/\bstock static bool:/stock static bool/g' "$file"

    # 12. stock const String: → stock const char
    sed -i 's/\bstock const String:/stock const char/g' "$file"

    # 13. const Float: → const float
    sed -i 's/\bconst Float:/const float/g' "$file"

    # 14. const String: → const char
    sed -i 's/\bconst String:/const char/g' "$file"

    # 15. Float: in function parameters → float
    sed -i 's/Float:\([a-zA-Z_][a-zA-Z0-9_]*\)\[\]/float \1[]/g' "$file"
    sed -i 's/Float:\([a-zA-Z_][a-zA-Z0-9_]*\)/float \1/g' "$file"

    # 16. bool: in function parameters → bool
    sed -i 's/bool:\([a-zA-Z_][a-zA-Z0-9_]*\)/bool \1/g' "$file"

    # 17. String: in function parameters → char
    sed -i 's/String:\([a-zA-Z_][a-zA-Z0-9_]*\)\[\]/char \1[]/g' "$file"
    sed -i 's/String:\([a-zA-Z_][a-zA-Z0-9_]*\)/char \1/g' "$file"

    # 18. Handle: in function parameters → Handle
    sed -i 's/Handle:\([a-zA-Z_][a-zA-Z0-9_]*\)/Handle \1/g' "$file"

    # 19. public Action: → public Action
    sed -i 's/\bpublic Action:/public Action/g' "$file"

    # 20. public Float: → public float
    sed -i 's/\bpublic Float:/public float/g' "$file"

    # 21. public bool: → public bool
    sed -i 's/\bpublic bool:/public bool/g' "$file"

    # 22. native Float: → native float
    sed -i 's/\bnative Float:/native float/g' "$file"

    # 23. native bool: → native bool
    sed -i 's/\bnative bool:/native bool/g' "$file"

    # 24. INVALID_HANDLE → null
    sed -i 's/\bINVALID_HANDLE\b/null/g' "$file"

    echo "Completed: $file"
}

# List of files to migrate (from user's request)
files=(
    "Teleport.inc"
    "screen.inc"
    "AirWeapons.inc"
    "Charge.inc"
    "giveammo.inc"
    "MissileAttack.inc"
    "SupplyDepot.inc"
    "Hallucinate.inc"
    "cloak.inc"
    "maxhealth.inc"
    "Stimpacks.inc"
    "menuitemt.inc"
    "trace.inc"
    "plugins.inc"
    "round_state.inc"
    "HealthParticle.inc"
    "client.inc"
    "Plague.inc"
    "RecallStructure.inc"
    "log.inc"
    "dump.inc"
    "Feedback.inc"
    "Levitation.inc"
    "MindControl.inc"
    "WarpIn.inc"
    "RecallSounds.inc"
    "Mutate.inc"
    "dissolve.inc"
    "burrow.inc"
    "SpeedBoost.inc"
    "clienttimer.inc"
    "Detector.inc"
    "DarkSwarm.inc"
    "invuln.inc"
    "UltimateFeedback.inc"
    "range.inc"
    "sounds.inc"
    "PsionicRage.inc"
)

# Migrate each file
for file in "${files[@]}"; do
    if [ -f "$file" ]; then
        migrate_file "$file"
    else
        echo "Warning: File not found: $file"
    fi
done

echo "Migration complete!"
