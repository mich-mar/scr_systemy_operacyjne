import random
import string


def generate_random_word(length):
    return ''.join(random.choice(string.ascii_lowercase) for _ in range(length))

def generate_random_sentence(word_count=10):
    words = [generate_random_word(random.randint(3, 8)) for _ in range(word_count)]
    return ' '.join(words)

random_sentence = generate_random_sentence()

# Zapis do pliku plik.txt
with open("plik.txt", "w") as file:
    file.write(random_sentence)

