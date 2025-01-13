import pygame

pygame.init()

WIDTH, HEIGHT = 800, 800
SQUARE_SIZE = WIDTH // 8

WHITE = (255, 255, 255)
BLACK = (118, 150, 86)

PIECE_IMAGES = {
    'P': pygame.image.load("python_gui/assets/wP.png"),
    'N': pygame.image.load("python_gui/assets/wN.png"),
    'B': pygame.image.load("python_gui/assets/wB.png"),
    'R': pygame.image.load("python_gui/assets/wR.png"),
    'Q': pygame.image.load("python_gui/assets/wQ.png"),
    'K': pygame.image.load("python_gui/assets/wK.png"),
    'p': pygame.image.load("python_gui/assets/bP.png"),
    'n': pygame.image.load("python_gui/assets/bK.png"),
    'b': pygame.image.load("python_gui/assets/bB.png"),
    'r': pygame.image.load("python_gui/assets/bR.png"),
    'q': pygame.image.load("python_gui/assets/bQ.png"),
    'k': pygame.image.load("python_gui/assets/bK.png"),
}

for key in PIECE_IMAGES:
    PIECE_IMAGES[key] = pygame.transform.scale(PIECE_IMAGES[key], (SQUARE_SIZE, SQUARE_SIZE))

screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Chess Visualization")

def draw_board():
    for rank in range(8):
        for file in range(8):
            color = WHITE if (rank + file) % 2 == 0 else BLACK
            pygame.draw.rect(screen, color, (file * SQUARE_SIZE, rank * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE))

def place_pieces(fen):
    piece_placement = fen.split()[0]
    rank = 0
    file = 0
    for char in piece_placement:
        if char == '/':
            rank += 1
            file = 0
        elif char.isdigit():
            file += int(char)
        else:
            piece = PIECE_IMAGES[char]
            x = file * SQUARE_SIZE
            y = rank * SQUARE_SIZE
            screen.blit(piece, (x, y))
            file += 1

def main(fen):
    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        draw_board()
        place_pieces(fen)
        pygame.display.flip()

    pygame.quit()

def get_fen_from_file(file_path):
    with open(file_path, "r") as file:
        return file.read().strip()

fen = get_fen_from_file("src/fen.txt")
main(fen)
