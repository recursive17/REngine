"""Вшивает картинку в exe как массив байт.

Файл кладётся как есть (сжатый PNG/JPG), декодирует его stb_image в
Texture::loadFromMemory. Поэтому массив маленький: сколько весит файл,
столько и байт.

Пример:
    python tools/embed_image.py src/textures/logo.png
    python tools/embed_image.py icon.jpg --name app_icon --out REngine/REngine/assets

Использование в коде:
    #include "assets/logo.h"
    textureManager.loadFromMemory("logo", re::assets::logoPng());
"""

import argparse
from pathlib import Path

from embed_common import embed

# форматы, которые понимает stb_image
IMAGE_EXT = {"png", "jpg", "jpeg", "bmp", "tga", "gif", "psd", "hdr", "pic", "pnm"}


def main() -> None:
    parser = argparse.ArgumentParser(description="Картинка -> .h/.cpp с массивом байт")
    parser.add_argument("input", type=Path, help="путь к картинке")
    parser.add_argument("--name", help="имя ассета (по умолчанию имя файла)")
    parser.add_argument("--out", type=Path, default=Path("REngine/REngine/assets"),
                        help="папка для .h/.cpp (по умолчанию REngine/REngine/assets)")
    args = parser.parse_args()

    embed(args.input, args.out, args.name, IMAGE_EXT, "image")


if __name__ == "__main__":
    main()
