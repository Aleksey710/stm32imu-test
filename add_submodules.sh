#!/bin/bash
set -e

#Git хранит submodule в 3 местах:

#место	что хранит
#.gitmodules	конфиг
#.git/index	gitlink (ВАЖНО)
#.git/modules	внутренний repo

echo "===> Hard reset submodule state"

git submodule deinit -f --all || true
rm -rf .git/modules

echo "===> Sync submodules list..."

SUBMODULES=(
    "https://github.com/joltwallet/esp_littlefs.git|esp32ahrs/components/esp_littlefs"
    "https://github.com/esp-idf-lib/i2cdev.git|esp32ahrs/components/esp-idf-lib/i2cdev"
    "https://github.com/esp-idf-lib/mpu6050.git|esp32ahrs/components/esp-idf-lib/mpu6050"
)

# 🧠 НЕ очищаем .gitmodules полностью!
touch .gitmodules

for entry in "${SUBMODULES[@]}"; do
    IFS='|' read -r REPO_URL SUBMODULE_PATH <<< "$entry"

    echo "===> Processing: $REPO_URL → $SUBMODULE_PATH"

    # 🔴 если уже есть запись — удаляем корректно
    if git config -f .gitmodules --get submodule."$SUBMODULE_PATH".url >/dev/null 2>&1; then
        echo "Removing existing submodule config: $SUBMODULE_PATH"
        git submodule deinit -f "$SUBMODULE_PATH" || true
        git rm -f "$SUBMODULE_PATH" || true
        rm -rf ".git/modules/$SUBMODULE_PATH" || true
    fi

	echo "Adding submodule..."

	# 🔴 1. удалить из index если есть
	git rm -f "$SUBMODULE_PATH" >/dev/null 2>&1 || true

	# 🔴 2. удалить рабочую папку
	rm -rf "$SUBMODULE_PATH"

	# 🔴 3. удалить внутренний git submodule
	rm -rf ".git/modules/$SUBMODULE_PATH"

	# 🔴 4. добавить заново
	git submodule add "$REPO_URL" "$SUBMODULE_PATH"

done

echo "===> Sync..."
git submodule sync --recursive

echo "===> Update..."
git submodule update --init --recursive

echo "===> Commit changes..."
git add .gitmodules

for entry in "${SUBMODULES[@]}"; do
    IFS='|' read -r _ SUBMODULE_PATH <<< "$entry"
    git add "$SUBMODULE_PATH" || true
done

git commit -m "Sync submodules"

echo "===> Done ✅"
