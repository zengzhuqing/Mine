#include <gtk/gtk.h>
#include <stdbool.h>

#define WIDTH 48
#define HEIGHT 24
#define MINE_NUM 128

#define UNKNOWN_LABEL "U"
#define BOMB_LABEL "B"
#define ZERO_LABEL " "
//#define DEBUG

typedef struct{
    int row_idx;
    int col_idx;
    GtkWidget *button;
} Mine_button;

typedef struct{
    bool mine_table[HEIGHT][WIDTH];
    int mine_num_table[HEIGHT][WIDTH];
    Mine_button buttons[HEIGHT][WIDTH];
} Mine;

typedef struct{
    int i;
    int j;
} Pair;

Mine mine;
bool known[HEIGHT][WIDTH];

static void init_mine_table()
{
    int i;
    int cur;
    int r, c;

    for(i = 0; i < MINE_NUM; )
    {
        cur = g_random_int_range(0, WIDTH * HEIGHT);
        r = cur / WIDTH;     
        c = cur % WIDTH;
        if(mine.mine_table[r][c])
        {
            continue;
        }
        else
        {
            mine.mine_table[r][c] = true; 
            i++;      
        } 
    }     
}

static int mine_num_for_each(int i, int j);

static void init_mine_num_table()
{
    int i, j;

    for(i = 0; i < HEIGHT; i++)
    {
        for(j = 0; j < WIDTH; j++)
            if(!mine.mine_table[i][j])
            {
                mine.mine_num_table[i][j] = mine_num_for_each(i, j); 
            }
    }
}

static int mine_num_for_each(int i, int j)
{
    int ans;
    g_assert(!mine.mine_table[i][j]);
   
    ans = 0;
    if((i - 1 >= 0) && (j - 1 >= 0) && mine.mine_table[i - 1][j - 1])
        ans++; 
    if((j - 1 >= 0) && mine.mine_table[i][j - 1])
        ans++;
    if((i + 1 < HEIGHT) && (j - 1 >= 0) && mine.mine_table[i + 1][j - 1])
        ans++;
    if((i + 1 < HEIGHT) && mine.mine_table[i + 1][j])
        ans++; 
    if((i + 1 < HEIGHT) && (j + 1 < WIDTH) && mine.mine_table[i + 1][j + 1])
        ans++;
    if((j + 1 < WIDTH) && mine.mine_table[i][j + 1])
        ans++;
    if((i - 1 >= 0) && (j + 1 < WIDTH) && mine.mine_table[i - 1][j + 1])
        ans++;
    if((i - 1 >= 0) && mine.mine_table[i - 1][j])
        ans++;

    return ans; 
}

static void expand_for_each(int ii, int jj)
{
    GQueue *unvisited;
    int i, j;
    Pair *a;
    g_assert(mine.mine_num_table[ii][jj] == 0);
    char buf[100];
    int num;

    if(known[ii][jj])
        return;
    
    /* eight neighbors, mine_num_table not zero is border, use bfs */
    unvisited = g_queue_new();    
    Pair *s = g_malloc(sizeof(Pair));
    s->i = ii;
    s->j = jj;  
    g_queue_push_tail(unvisited,s);
    while(!g_queue_is_empty(unvisited))
    {
        Pair *cur = (Pair *)g_queue_peek_head(unvisited);
        g_queue_pop_head(unvisited);
      
        i = cur->i;
        j = cur->j; 
        gtk_button_set_label(GTK_BUTTON(mine.buttons[i][j].button), ZERO_LABEL); 
        if((i - 1 >= 0) && (j - 1 >= 0) && !known[i - 1][j - 1])
        {
            known[i - 1][j - 1] = true;
            num = mine.mine_num_table[i - 1][j - 1];
            g_assert(num >= 0);
            if(num == 0)
            {
                a = g_malloc(sizeof(Pair));
                a->i = i - 1;
                a->j = j - 1;
                g_queue_push_tail(unvisited,a);
            }
            else
            {
                snprintf(buf, sizeof(buf), "%d", num); 
                gtk_button_set_label(GTK_BUTTON(mine.buttons[i - 1][j - 1].button), buf); 
            } 
        } 
        if((j - 1 >= 0) && !known[i][j - 1])
        {
            known[i][j - 1] = true;
            num = mine.mine_num_table[i][j - 1];
            g_assert(num >= 0);
            if(num == 0)
            {
                a = g_malloc(sizeof(Pair));
                a->i = i;
                a->j = j - 1;
                g_queue_push_tail(unvisited,a);
            }
            else
            {
                snprintf(buf, sizeof(buf), "%d", num); 
                gtk_button_set_label(GTK_BUTTON(mine.buttons[i][j - 1].button), buf); 
            } 
        } 
        if((i + 1 < HEIGHT) && (j - 1 >= 0) && !known[i + 1][j - 1])
        {
            known[i + 1][j - 1] = true;
            num = mine.mine_num_table[i + 1][j - 1];
            g_assert(num >= 0);
            if(num == 0)
            {
                a = g_malloc(sizeof(Pair));
                a->i = i + 1;
                a->j = j - 1;
                g_queue_push_tail(unvisited,a);
            }
            else
            {
                snprintf(buf, sizeof(buf), "%d", num); 
                gtk_button_set_label(GTK_BUTTON(mine.buttons[i + 1][j - 1].button), buf); 
            } 
        }
        if((i + 1 < HEIGHT) && !known[i + 1][j])
        {
            known[i + 1][j] = true;
            num = mine.mine_num_table[i + 1][j];
            g_assert(num >= 0);
            if(num == 0)
            {
                a = g_malloc(sizeof(Pair));
                a->i = i + 1;
                a->j = j;
                g_queue_push_tail(unvisited,a);
            }
            else
            {
                snprintf(buf, sizeof(buf), "%d", num); 
                gtk_button_set_label(GTK_BUTTON(mine.buttons[i + 1][j].button), buf); 
            }
        } 
        if((i + 1 < HEIGHT) && (j + 1 < WIDTH) && !known[i + 1][j + 1])
        {
            known[i + 1][j + 1] = true;
            num = mine.mine_num_table[i + 1][j + 1];
            g_assert(num >= 0);
            if(num == 0)
            {
                a = g_malloc(sizeof(Pair));
                a->i = i + 1;
                a->j = j + 1;
                g_queue_push_tail(unvisited,a);
            }
            else
            {
                snprintf(buf, sizeof(buf), "%d", num); 
                gtk_button_set_label(GTK_BUTTON(mine.buttons[i + 1][j + 1].button), buf); 
            }
        }
        if((j + 1 < WIDTH) && !known[i][j + 1])
        {
            known[i][j + 1] = true;
            num = mine.mine_num_table[i][j + 1];
            g_assert(num >= 0);
            if(num == 0)
            {
                a = g_malloc(sizeof(Pair));
                a->i = i;
                a->j = j + 1;
                g_queue_push_tail(unvisited,a);
            }
            else
            {
                snprintf(buf, sizeof(buf), "%d", num); 
                gtk_button_set_label(GTK_BUTTON(mine.buttons[i][j + 1].button), buf); 
            }
        }
        if((i - 1 >= 0) && (j + 1 < WIDTH) && !known[i - 1][j + 1])
        {
            known[i - 1][j + 1] = true;
            num = mine.mine_num_table[i - 1][j + 1];
            g_assert(num >= 0);
            if(num == 0)
            {
                a = g_malloc(sizeof(Pair));
                a->i = i - 1;
                a->j = j + 1;
                g_queue_push_tail(unvisited,a);
            }
            else
            {
                snprintf(buf, sizeof(buf), "%d", num); 
                gtk_button_set_label(GTK_BUTTON(mine.buttons[i - 1][j + 1].button), buf); 
            }
        }
        if((i - 1 >= 0) && !known[i - 1][j])
        {
            known[i - 1][j] = true;
            num = mine.mine_num_table[i - 1][j];
            g_assert(num >= 0);
            if(num == 0)
            {
                a = g_malloc(sizeof(Pair));
                a->i = i - 1;
                a->j = j;
                g_queue_push_tail(unvisited,a);
            }
            else
            {
                snprintf(buf, sizeof(buf), "%d", num); 
                gtk_button_set_label(GTK_BUTTON(mine.buttons[i - 1][j].button), buf); 
            }
        } 

        g_free(cur);
        cur == NULL; 
    }

    g_queue_free(unvisited);
    unvisited = NULL; 
}

static void bomb()
{
    int i, j;

    for(i = 0; i < HEIGHT; i++)
    {
        for(j = 0; j < WIDTH; j++)
        {
            if(mine.mine_table[i][j])
            {
               gtk_button_set_label(GTK_BUTTON(mine.buttons[i][j].button), BOMB_LABEL); 
            }
        }
    }
}

static void on_button_click(GtkWidget *widget, gpointer data)
{
    Mine_button *cur = (Mine_button*)data;
    char buf[100];
    int num;

    if(mine.mine_table[cur->row_idx][cur->col_idx])
    {
        bomb(); 
        g_print ("Game over!\n");
    }
    else
    {
        num = mine.mine_num_table[cur->row_idx][cur->col_idx];
        
        if(num != 0)
        {
            snprintf(buf, sizeof(buf), "%d", num); 
            gtk_button_set_label(GTK_BUTTON(mine.buttons[cur->row_idx][cur->col_idx].button), buf); 
        }
        else
        {
            g_print ("Expand it!\n");
            expand_for_each(cur->row_idx, cur->col_idx);
        }
    }
}

int main(int argc, char *argv[])
{
    /* GtkWidget is the storage type for widgets */
    GtkWidget *window;
    GtkWidget *grid;
    
    int i, j;

    init_mine_table();
    init_mine_num_table();

    gtk_init(&argc, &argv);

/*DEBUG */
#ifdef DEBUG
    for(i = 0; i < HEIGHT; i++)
    {
        for(j = 0; j < WIDTH; j++)
            printf("%d ", mine.mine_table[i][j]);
        printf("\n");
    }
    for(i = 0; i < HEIGHT; i++)
    {
        for(j = 0; j < WIDTH; j++)
            printf("%d ", mine.mine_num_table[i][j]);
        printf("\n");
    }
#endif

    /* create a new window, and set its title */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW (window), "Mine");

    (void)g_signal_connect (window, "delete-event", G_CALLBACK(gtk_widget_destroy), NULL);

    (void)g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    gtk_container_set_border_width (GTK_CONTAINER (window), 10);

    grid = gtk_grid_new();
   
    /* contruct mine grid */
    for(i = 0; i < HEIGHT; i++)
    {
        for(j = 0; j < WIDTH; j++)
        {
            mine.buttons[i][j].row_idx = i;
            mine.buttons[i][j].col_idx = j;
            mine.buttons[i][j].button = gtk_button_new_with_label(UNKNOWN_LABEL);
            gtk_grid_attach(GTK_GRID(grid), mine.buttons[i][j].button, j, i, 1, 1);  
            (void)g_signal_connect(mine.buttons[i][j].button, "clicked", 
                G_CALLBACK(on_button_click), &(mine.buttons[i][j]));
        }
    }
       
    gtk_container_add(GTK_CONTAINER(window), grid);
    
    /* show */
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
