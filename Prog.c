#include <stdio.h>
#include <stdlib.h>

#define WIDTH 60
#define HEIGHT 20
#define MAX_SHAPES 100

// Define types of shapes
typedef enum { LINE, RECTANGLE, CIRCLE, TRIANGLE } ShapeType;

// Structure to hold shape details
typedef struct {
    int id;
    ShapeType type;
    int active; // 1 if active, 0 if deleted
    int x1, y1; // Point 1 / Center for Circle
    int x2, y2; // Point 2 / Width-Height for Rect
    int x3, y3; // Point 3 for Triangle
    int radius; // Radius for Circle
} Shape;

// Global structures
char canvas[HEIGHT][WIDTH];
Shape shapes[MAX_SHAPES];
int shape_count = 0;
int next_id = 1;

// Helper function to clear input buffer
void clear_buffer() {
    while (getchar() != '\n');
}

// Function to initialize/clear the canvas array
void clear_canvas() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            canvas[i][j] = '_';
        }
    }
}

// Function to put a safe pixel on the canvas
void put_pixel(int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = '*';
    }
}

// --- DRAWING ALGORITHMS ---

// Bresenham's Line Algorithm
void draw_line(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        put_pixel(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void draw_rectangle(int x1, int y1, int x2, int y2) {
    draw_line(x1, y1, x2, y1); // Top
    draw_line(x2, y1, x2, y2); // Right
    draw_line(x2, y2, x1, y2); // Bottom
    draw_line(x1, y2, x1, y1); // Left
}

void plot_circle_points(int xc, int yc, int x, int y) {
    put_pixel(xc + x, yc + y);
    put_pixel(xc - x, yc + y);
    put_pixel(xc + x, yc - y);
    put_pixel(xc - x, yc - y);
    put_pixel(xc + y, yc + x);
    put_pixel(xc - y, yc + x);
    put_pixel(xc + y, yc - x);
    put_pixel(xc - y, yc - x);
}

// Bresenham's Circle Algorithm
void draw_circle(int xc, int yc, int r) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    plot_circle_points(xc, yc, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        plot_circle_points(xc, yc, x, y);
    }
}

void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}

// --- EDITOR CORE FUNCTIONS ---

// Render active shapes onto the 2D character array
void render_canvas() {
    clear_canvas();
    for (int i = 0; i < shape_count; i++) {
        if (!shapes[i].active) continue;

        switch (shapes[i].type) {
            case LINE:
                draw_line(shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2);
                break;
            case RECTANGLE:
                draw_rectangle(shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2);
                break;
            case CIRCLE:
                draw_circle(shapes[i].x1, shapes[i].y1, shapes[i].radius);
                break;
            case TRIANGLE:
                draw_triangle(shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2, shapes[i].x3, shapes[i].y3);
                break;
        }
    }
}

// Display the 2D array to the console
void display_canvas() {
    printf("\n--- CANVAS (%dx%d) ---\n", WIDTH, HEIGHT);
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%c", canvas[i][j]);
        }
        printf("\n");
    }
    printf("------------------------\n");
}

void list_shapes() {
    printf("\n--- Active Objects List ---\n");
    int count = 0;
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].active) {
            count++;
            printf("ID: %d | ", shapes[i].id);
            if (shapes[i].type == LINE) printf("Line [(%d,%d) to (%d,%d)]\n", shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2);
            else if (shapes[i].type == RECTANGLE) printf("Rectangle [(%d,%d) to (%d,%d)]\n", shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2);
            else if (shapes[i].type == CIRCLE) printf("Circle [Center:(%d,%d), Radius:%d]\n", shapes[i].x1, shapes[i].y1, shapes[i].radius);
            else if (shapes[i].type == TRIANGLE) printf("Triangle [(%d,%d), (%d,%d), (%d,%d)]\n", shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2, shapes[i].x3, shapes[i].y3);
        }
    }
    if (count == 0) printf("No shapes found.\n");
}

// User prompts to add a shape
void add_shape() {
    if (shape_count >= MAX_SHAPES) {
        printf("Error: Maximum shape storage reached!\n");
        return;
    }

    int choice;
    printf("\nChoose shape to add:\n1. Line\n2. Rectangle\n3. Circle\n4. Triangle\nChoice: ");
    if(scanf("%d", &choice) != 1) { clear_buffer(); return; }

    Shape s;
    s.id = next_id++;
    s.active = 1;

    if (choice == 1) {
        s.type = LINE;
        printf("Enter start X Y and end X Y (e.g. 5 5 20 15): ");
        scanf("%d %d %d %d", &s.x1, &s.y1, &s.x2, &s.y2);
    } else if (choice == 2) {
        s.type = RECTANGLE;
        printf("Enter top-left X Y and bottom-right X Y (e.g. 10 2 30 12): ");
        scanf("%d %d %d %d", &s.x1, &s.y1, &s.x2, &s.y2);
    } else if (choice == 3) {
        s.type = CIRCLE;
        printf("Enter center X Y and radius (e.g. 30 10 5): ");
        scanf("%d %d %d", &s.x1, &s.y1, &s.radius);
    } else if (choice == 4) {
        s.type = TRIANGLE;
        printf("Enter X Y for 3 points (e.g. 10 18 20 5 30 18): ");
        scanf("%d %d %d %d %d %d", &s.x1, &s.y1, &s.x2, &s.y2, &s.x3, &s.y3);
    } else {
        printf("Invalid choice!\n");
        return;
    }

    shapes[shape_count++] = s;
    printf("Shape added successfully with ID %d!\n", s.id);
}

// Delete shape by setting active flag to 0
void delete_shape() {
    list_shapes();
    int id, found = 0;
    printf("\nEnter the ID of the object to delete: ");
    if(scanf("%d", &id) != 1) { clear_buffer(); return; }

    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == id && shapes[i].active) {
            shapes[i].active = 0;
            found = 1;
            printf("Object %d deleted.\n", id);
            break;
        }
    }
    if (!found) printf("Object ID not found.\n");
}

// Modify an existing shape properties
void modify_shape() {
    list_shapes();
    int id, found = 0;
    printf("\nEnter the ID of the object to modify: ");
    if(scanf("%d", &id) != 1) { clear_buffer(); return; }

    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == id && shapes[i].active) {
            found = 1;
            printf("Modifying shape ID %d:\n", id);
            if (shapes[i].type == LINE) {
                printf("Enter new start X Y and end X Y: ");
                scanf("%d %d %d %d", &shapes[i].x1, &shapes[i].y1, &shapes[i].x2, &shapes[i].y2);
            } else if (shapes[i].type == RECTANGLE) {
                printf("Enter new top-left X Y and bottom-right X Y: ");
                scanf("%d %d %d %d", &shapes[i].x1, &shapes[i].y1, &shapes[i].x2, &shapes[i].y2);
            } else if (shapes[i].type == CIRCLE) {
                printf("Enter new center X Y and radius: ");
                scanf("%d %d %d", &shapes[i].x1, &shapes[i].y1, &shapes[i].radius);
            } else if (shapes[i].type == TRIANGLE) {
                printf("Enter new X Y for 3 points: ");
                scanf("%d %d %d %d %d %d", &shapes[i].x1, &shapes[i].y1, &shapes[i].x2, &shapes[i].y2, &shapes[i].x3, &shapes[i].y3);
            }
            printf("Shape modified successfully!\n");
            break;
        }
    }
    if (!found) printf("Object ID not found.\n");
}

int main() {
    int choice = 0;

    while (1) {
        render_canvas();
        display_canvas();

        printf("\n=== 2D GRAPHICS EDITOR ===\n");
        printf("1. Add Object\n");
        printf("2. Delete Object\n");
        printf("3. Modify Object\n");
        printf("4. List Objects\n");
        printf("5. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid Input! Please enter a number.\n");
            clear_buffer();
            continue;
        }

        switch (choice) {
            case 1: add_shape(); break;
            case 2: delete_shape(); break;
            case 3: modify_shape(); break;
            case 4: list_shapes(); break;
            case 5: printf("Exiting editor...\n"); return 0;
            default: printf("Invalid Choice!\n");
        }
    }
    return 0;
}