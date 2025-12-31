#!/bin/bash
# SourcePawn 1.13 Migration Script
# Migrates all .inc files in W3SIncs directory to modern syntax

echo "Starting SourcePawn 1.13 migration..."

# List of files to migrate (excluding War3Source_Interface.inc which is already done)
files=(
  "War3Source_TF2_Interface.inc"
  "War3Source_CS_Interface.inc"
  "War3Source_L4D_Interface.inc"
  "SuperHero_Interface.inc"
  "War3Source_Attributes.inc"
  "War3Source_AttributeBuffs.inc"
  "War3Source_Aura.inc"
  "aura.inc"
  "War3Source_Buffs.inc"
  "War3Source_BuffSystem.inc"
  "War3Source_Wards.inc"
  "War3Source_Effects.inc"
  "War3Source_SkillEffects.inc"
  "War3Source_Constants.inc"
  "constants.inc"
  "War3Source_Config.inc"
  "War3Source_Events.inc"
  "War3Source_Notifications.inc"
  "War3Source_Health.inc"
  "War3Source_XP_Gold.inc"
  "War3Source_Currency.inc"
  "War3Source_Races.inc"
  "War3Source_Shopitems.inc"
  "War3Source_Shopitems2.inc"
  "War3Source_Gamecheck.inc"
  "War3Source_PrecacheDownload.inc"
  "War3Source_Logging.inc"
  "War3Source_Bots.inc"
  "mana.inc"
  "colors.inc"
  "sdkhooks.inc"
  "revantools.inc"
  "stocks_misc.inc"
  "stocks_precache.inc"
)

for file in "${files[@]}"; do
  if [ -f "$file" ]; then
    echo "Migrating $file..."

    # Apply comprehensive transformations
    sed -i '
      # Variable declarations
      s/\<new bool:/bool /g
      s/\<new Float:/float /g
      s/\<new String:/char /g
      s/\<new Handle:/Handle /g
      s/\<decl String:/char /g
      s/\<decl Float:/float /g
      s/\<new \([a-zA-Z_][a-zA-Z0-9_]*\)\([^:]\)/int \1\2/g

      # Static/const declarations
      s/\<static Float:/static float /g
      s/\<static bool:/static bool /g
      s/\<static String:/static char /g
      s/\<static Handle:/static Handle /g
      s/\<static const String:/static const char /g
      s/\<static const Float:/static const float /g
      s/\<const String:/const char /g
      s/\<const Float:/const float /g

      # Function return types and parameters
      s/stock Float:/stock float /g
      s/stock bool:/stock bool /g
      s/stock Handle:/stock Handle /g
      s/stock String:/stock char /g
      s/native Float:/native float /g
      s/native bool:/native bool /g
      s/native Handle:/native Handle /g
      s/forward Float:/forward float /g
      s/forward bool:/forward bool /g
      s/forward Handle:/forward Handle /g
      s/forward Action:/forward Action /g
      s/public Float:/public float /g
      s/public bool:/public bool /g
      s/public Handle:/public Handle /g
      s/public Action:/public Action /g

      # Function parameters
      s/Float:\([a-zA-Z_][a-zA-Z0-9_]*\)/float \1/g
      s/bool:\([a-zA-Z_][a-zA-Z0-9_]*\)/bool \1/g
      s/Handle:\([a-zA-Z_][a-zA-Z0-9_]*\)/Handle \1/g
      s/String:\([a-zA-Z_][a-zA-Z0-9_]*\)\[\]/char[] \1/g
      s/const String:\([a-zA-Z_][a-zA-Z0-9_]*\)\[\]/const char[] \1/g
      s/any:\.\.\./any .../g

      # Handle replacement
      s/INVALID_HANDLE/null/g
      s/Handle:0/null/g

      # Enum and tagged types
      s/W3Var:/W3Var /g
      s/W3EVENT:/W3EVENT /g
      s/W3DENY:/W3DENY /g
      s/War3Immunity:/War3Immunity /g
      s/W3HintPriority:/W3HintPriority /g
      s/CooldownType:/CooldownType /g
      s/SkillDependency:/SkillDependency /g
      s/ValveGameEnum:/ValveGameEnum /g
      s/Plugin:/Plugin /g
      s/APLRes:/APLRes /g
      s/Function:/Function /g
      s/W3PlayerProp:/W3PlayerProp /g

      # Common patterns
      s/for(new /for(int /g
      s/for (new /for (int /g
    ' "$file"

    echo "  ✓ $file migrated"
  else
    echo "  ✗ $file not found"
  fi
done

echo "Migration complete!"
echo "Total files processed: ${#files[@]}"
