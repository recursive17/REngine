"""Вшивает звук/музыку в exe как массив байт.

Файл кладётся как есть (сжатый), декодирует его miniaudio в
Sound::loadFromMemory. Для музыки лучше .mp3: .wav не сжат,
и минута звука это ~10 МБ массива.

Пример:
    python tools/embed_audio.py src/audio/click.wav
    python tools/embed_audio.py theme.mp3 --name main_theme

Использование в коде:
    #include "assets/click.h"
    soundManager.loadFromMemory("click", re::assets::clickWav());
    soundManager.play("click");
"""

import argparse
from pathlib import Path

from embed_common import embed

# форматы, которые miniaudio декодирует без доп. библиотек (.ogg требует stb_vorbis)
AUDIO_EXT = {"wav", "mp3", "flac"}


def main() -> None:
    parser = argparse.ArgumentParser(description="Аудио -> .h/.cpp с массивом байт")
    parser.add_argument("input", type=Path, help="путь к аудиофайлу")
    parser.add_argument("--name", help="имя ассета (по умолчанию имя файла)")
    parser.add_argument("--out", type=Path, default=Path("REngine/REngine/assets"),
                        help="папка для .h/.cpp (по умолчанию REngine/REngine/assets)")
    args = parser.parse_args()

    embed(args.input, args.out, args.name, AUDIO_EXT, "audio")


if __name__ == "__main__":
    main()
