#!/bin/bash

# Script to migrate SourcePawn files to 1.13 syntax
# Usage: ./migrate_remaining.sh

# List of files to migrate (full paths)
files=(
    "/home/user/SourceCraftModernized/scripting/SourceCraft/Burrow.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/MindControl.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/War3Helper.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/Broodling.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/Baneling.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/DarkTemplar.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/TheHunter.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/Necromancer.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/Al-Qaeda.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/DarkArchon.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/SickFarter.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/PlagueInfect.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/UAW.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/Infested.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/Battlecruiser.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/War3Source_Engine_Wards.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/War3Source_Engine_Aura.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/War3Source_Engine_BuffSystem.sp"
    "/home/user/SourceCraftModernized/scripting/SourceCraft/WCX_Engine_Crit.sp"
)

echo "Starting migration of ${#files[@]} files..."

for file in "${files[@]}"; do
    if [ ! -f "$file" ]; then
        echo "Skipping $file (not found)"
        continue
    fi

    echo "Migrating: $file"

    # Apply all transformations
    sed -i 's/public Plugin:myinfo/public Plugin myinfo/g' "$file"
    sed -i 's/public Action:/public Action /g' "$file"
    sed -i 's/public bool:/public bool /g' "$file"

    # Type transformations
    sed -i 's/new bool:/bool /g' "$file"
    sed -i 's/new Float:/float /g' "$file"
    sed -i 's/new String:/char /g' "$file"
    sed -i 's/decl String:/char /g' "$file"
    sed -i 's/new Handle:/Handle /g' "$file"
    sed -i 's/new const String:/static const char /g' "$file"
   sed -i 's/const String:\([a-zA-Z0-9_]*\)\[\]/const char \1[]/g' "$file"
    sed -i 's/Float:\([a-zA-Z0-9_]*\)/float \1/g' "$file"
    sed -i 's/bool:\([a-zA-Z0-9_]*\)/bool \1/g' "$file"
    sed -i 's/String:\([a-zA-Z0-9_]*\)/char \1/g' "$file"
    sed -i 's/Handle:\([a-zA-Z0-9_]*\)/Handle \1/g' "$file"

    # INVALID_HANDLE -> null
    sed -i 's/INVALID_HANDLE/null/g' "$file"

    # for loops
    sed -i 's/for(new /for(int /g' "$file"
    sed -i 's/for (new /for (int /g' "$file"

    # new -> int/float/etc as first declaration in block
    sed -i 's/^\([[:space:]]*\)new \([a-zA-Z0-9_]*\) =/\1int \2 =/g' "$file"
    sed -i 's/^\([[:space:]]*\)new \([a-zA-Z0-9_]*\);/\1int \2;/g' "$file"
    sed -i 's/^\([[:space:]]*\)new \([a-zA-Z0-9_]*\)\[/\1int \2[/g' "$file"
    sed -i 's/^\([[:space:]]*\)decl \([a-zA-Z0-9_]*\)\[/\1int \2[/g' "$file"

    echo "  - Completed: $file"
done

echo "Migration complete!"
