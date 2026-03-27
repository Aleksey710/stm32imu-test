Установка

# 1. Установить зависимости Python
sudo apt update
sudo apt install python3 python3-pip python3-venv -y

Установка MkDocs в проект esp32imu
# перейти в директорию проекта
cd ~/Projects/esp32imu

Вариант с твоей структурой ~/.virtualenvs
🔹 1. Создать окружение
python3 -m venv /home/grey/.virtualenvs/esp32imu
🔹 2. Активировать
source /home/grey/.virtualenvs/esp32imu/bin/activate
🔹 3. Перейти в проект
cd ~/Projects/esp32imu
🔹 4. Установить зависимости
pip install --upgrade pip

pip install \
mkdocs \
mike \
pymdown-extensions \
mkdocs-material \
mkdocs-video \
mkdocs-awesome-pages-plugin \
mkdocs-macros-plugin \
mkdocs-mermaid2-plugin \
mkdocs-puml \
mkdocs-plantuml \
mkdocs-git-revision-date-plugin \
mkdocs-pdf-export-plugin \


▶️ Запуск
source /home/grey/.virtualenvs/esp32imu/bin/activate
cd ~/Projects/esp32imu
mkdocs serve


💡 Удобство (очень рекомендую)
Добавь alias:

echo "alias espdocs='source /home/grey/.virtualenvs/esp32imu/bin/activate && cd ~/Projects/esp32imu'" >> ~/.bashrc
source ~/.bashrc

Теперь:

espdocs
mkdocs serve
📁 Итоговая структура
/home/grey/.virtualenvs/esp32imu   # окружение
~/Projects/esp32imu/               # проект
 ├── mkdocs.yml
 └── docs/
🚀 Альтернатива (ещё удобнее)

Если используешь virtualenvwrapper:

mkvirtualenv esp32imu
workon esp32imu
🧠 Важно понимать

окружение → хранится отдельно (~/.virtualenvs)

проект → отдельно (~/Projects)

это правильная архитектура (как в больших проектах)

#-----------------------------------------------------------------------
Сделать папку репозиторием Git (рекомендовано)

Если проект не в Git, просто инициализируй его:

cd ~/Projects/esp32imu/esp32imu-docs
git init
git add .
git commit -m "Initial commit"

Теперь плагин сможет получить даты изменений, и mkdocs build пройдет успешно.
#-----------------------------------------------------------------------
#-----------------------------------------------------------------------
Cоздайте документацию:

mkdocs build
В результате будет создана новая директория с именем site

#-----------------------------------------------------------------------
MkDocs поставляется со встроенным сервером разработки, который позволяет просматривать документацию во время работы над ней. 
Убедитесь, что вы находитесь в той же директории, что и mkdocs.yml файл конфигурации, 
а затем запустите сервер, выполнив mkdocs serve команду:

mkdocs serve

Откройте в браузере адрес http://127.0.0.1:8000/ , и вы увидите главную страницу по умолчанию.

#-----------------------------------------------------------------------
Удаляем старую:

sudo apt remove wkhtmltopdf


Скачиваем правильную (с патчами):
https://wkhtmltopdf.org/downloads.html

wget https://github.com/wkhtmltopdf/packaging/releases/download/0.12.6.1-3/wkhtmltox_0.12.6.1-3.jammy_amd64.deb

Устанавливаем:

sudo apt install ./wkhtmltox_0.12.6.1-3.jammy_amd64.deb

Примеры использования
🔹 1. Одна страница → PDF
wkhtmltopdf site/index.html doc.pdf


🔹 2. Несколько страниц в один PDF
wkhtmltopdf \
site/index.html \
site/imu/index.html \
site/control/index.html \
full.pdf


🔹 3. С настройками
wkhtmltopdf \
--enable-local-file-access \
--margin-top 10mm \
--margin-bottom 10mm \
--footer-center "[page]" \
site/index.html \
output.pdf

--enable-local-file-access   # чтобы грузились css/js
--print-media-type          # печатные стили
--outline                   # оглавление
--footer-center "[page]"    # номера страниц

Альтернатива (если захочешь позже)

pandoc → лучше для “книг”

mkdocs-material + print → почти как docs PX4

weasyprint → современнее, чем wkhtmltopdf
#-----------------------------------------------------------------------
Выкладывание на сайт
scp -r ./site user@host:/path/to/server/root
