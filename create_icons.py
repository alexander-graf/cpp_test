import base64
import os
import shutil

# Backup-Ordner erstellen falls nicht vorhanden
if not os.path.exists('icons_backup'):
    os.makedirs('icons_backup')

# Vorhandene Icons sichern
if os.path.exists('icons/refresh.png'):
    shutil.copy2('icons/refresh.png', 'icons_backup/refresh.png')
if os.path.exists('icons/clear.png'):
    shutil.copy2('icons/clear.png', 'icons_backup/clear.png')

# Icons als Base64-Strings
icons = {
    'folder': 'iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAOxAAADsQBlSsOGwAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAADdSURBVDiNpZKxCsIwEIa/S1JbcHDQwUEQfB5fwcnZF3B09g0cfQRfwMnJF3BycnBwEAQHhw6KtGkuToVgk7Yq/pDj7u6/+/PfCf+UeG84na3yJIl7WksiEYGIICIgAgAiQhRFjVprZ71aLG8GQXdYVRVlWVJVFXVdU9c1xhiMMRhjsNZireXQFAf7qqgKay3WWqy1OOew1uGcwzmHcw5rLc45vPcYY6jrmrIsKcuSoiiYL9fj72H8JkmSxN77B/Hee+89fd3tABhj8N7jnMM5R1VVOI/p9wdJp5O+/UZfyQ2DU0G6+TqxvwAAAABJRU5ErkJggg==',
    'save': 'iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAOxAAADsQBlSsOGwAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAADJSURBVDiNpZKxCsIwEIa/S2Jb6CCCQwdBEHwEX8HJ2RdwdPYNHH0EX8DJ0TdwcnJwcBAEB4cOitQ0zXVQDE3TNij+kOPu7r/78wv/lHpvOJ2t8iSJe1pLIhGBiCAiIAIgIkRR1Ki11lmvFsubQdAdVlVFWZZUVUVd1xhjMMZgjMFai7UWay2HY3Gwr4qqsNZircVai3MOax3OOZxzOOew1uGcwxhDXdeUZUlZlhRFwXy5Hn8P429JkiTx3j+I9957T193OwDGGLz3/wRvyQ2pGEBQqGpKzQAAAABJRU5ErkJggg==',
    'delete': 'iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAOxAAADsQBlSsOGwAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAADRSURBVDiNpZKxCoMwEIb/S2JtcXBwsIODIOjg4Cv4Ck7OvoCjs2/g6CP4Ak5OvoCTk4ODgyA4dOigSE3TXAelGJO0KP6Q4+7uv/vzC/+UeG84na3yJIl7WksiEYGIICIgAiAiRFHUqLXWWa8Wy5tB0B1WVUVZllRVRV3XGGMwxmCMwVqLtRZrLYdjcbCvCmOw1mKtxTmHtQ7nHM45nHNY63DO4ZzDGENd15RlSVmWFEXBfLkefwvjb0mSJPHeP4j33nvv6etuB8AYg/f+n+ANl7tBUEi6qnEAAAAASUVORK5CYII=',
    'help': 'iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAOxAAADsQBlSsOGwAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAAFCSURBVDiNpZKxSgNBEIb/2b29JIdnTEQQFEQQFCwEQbC0s7cXn8DOR7CxsRMsBAsLwUoQRBQRxEIUFINGvdzd7c5YXEwwudwr/GCY2Zn5/5n5l/CfQ78NTg6vFuuT2YxQSgoAkFJaPHy8u7zZ2Xj8qZb+FhRy5fnRTGnZ93xorSCVgJACQkiMZSeLOd9b3T/dvPgSTGbLi77vQWgBIQSEkBBSQUoFIRUm81NjE7n80qcgpZUvhA+hBaSUUEpBKQ0pFaSUUFrA8z3vg0BrDSkEhPAhpYKSGkrK5CslpFJQUsDzvG8CqQU8z4dSAkpKaKXiFhJSSQgp4Xk+oigCEQMRAzEGxhgYY2CMgzEGxhgYZ2CMgXMGxjk452CcgXEO7nBwzsE4A+McjHNwJ17/tZGM8Q9BXJGBMQaHO+CMw+EczOFgjIFxBsYYOGfgjMF13T9fY2KMvQO4xF9QFB5q7QAAAABJRU5ErkJggg==',
    'file': 'iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAOxAAADsQBlSsOGwAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAAC/SURBVDiNpZKxCoMwEIb/S2JtcXBwsIODIOjg4Cv4Ck7OvoCjs2/g6CP4Ak5OvoCTk4ODgyA4dOigSE3TXAelGJO0KP6Q4+7uv/vzC/+UeG84na3yJIl7WksiEYGIICIgAiAiRFHUqLXWWa8Wy5tB0B1WVUVZllRVRV3XGGMwxmCMwVqLtRZrLYdjcbCviqqqsNZircU5h7UO5xzOOZxzWOtwzuGcwxhDXdeUZUlZlhRFwXy5Hn8L429JkiTx3r8BUoVBUEQFxRMAAAAASUVORK5CYII='
}

def save_icon(base64_string, filename):
    # Überspringe refresh.png und clear.png
    if filename in ['icons/refresh.png', 'icons/clear.png']:
        return
        
    # Base64-String dekodieren
    binary_data = base64.b64decode(base64_string)
    
    # Datei speichern
    with open(filename, 'wb') as f:
        f.write(binary_data)

# Icons Ordner erstellen falls nicht vorhanden
if not os.path.exists('icons'):
    os.makedirs('icons')

# Icons speichern
for name, base64_string in icons.items():
    filepath = os.path.join('icons', f'{name}.png')
    save_icon(base64_string, filepath)
    print(f'Icon gespeichert: {filepath}') 