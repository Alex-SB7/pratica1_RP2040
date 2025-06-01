#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "hardware/timer.h"

//arquivo .pio
#include "pio_matrix.pio.h"

//número de LEDs
#define NUM_PIXELS 25

//pino de saída
#define OUT_PIN 7

//botão de interupção
const uint button_0 = 5;
const uint button_1 = 6;

//variáveis do PIO
PIO pio = pio0; 
bool ok;
uint16_t i;
uint32_t valor_led;
double r = 0.0, b = 0.0 , g = 0.0;
uint sm;

//flags de interrupção
int p_flag_1 = 0;
int p_flag_2 = 0;

// Variáveis para debouncing
static volatile uint a = 1;
static volatile uint ab = 1;
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)


//ANIMAÇÃO 0
double desenho_0[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                          0.0, 0.0, 0.0, 0.0, 0.0, 
                          0.0, 0.0, 0.0, 0.0, 0.0,
                          0.0, 0.0, 0.0, 0.0, 0.0,
                          0.0, 0.0, 0.0, 0.0, 0.0};


//ANIMAÇÃO A
//Vetores para criar desenho 01 
double desenho_a_1_r[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.1, 0.1, 0.1, 0.0, 
                              0.0, 0.1, 0.1, 0.1, 0.0,
                              0.0, 0.1, 0.1, 0.1, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0};

double desenho_a_1_g[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0, 
                              0.0, 0.0, 0.1, 0.0, 0.0,
                              0.0, 0.1, 0.1, 0.1, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0};

double desenho_a_1_b[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0, 
                              0.0, 0.0, 0.1, 0.0, 0.0,
                              0.0, 0.1, 0.1, 0.1, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0};

//Vetores para criar desenho 02 
double desenho_a_2_r[25] =   {0.0, 0.0, 0.0, 0.0, 1.0,
                              0.0, 0.0, 0.0, 0.0, 0.0, 
                              0.0, 0.0, 1.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 1.0, 0.0,
                              1.0, 0.0, 0.0, 0.0, 0.0};

double desenho_a_2_g[25] =   {0.5, 0.5, 0.5, 0.0, 0.1,
                              0.0, 0.5, 0.5, 0.0, 0.0, 
                              0.0, 0.0, 0.1, 0.5, 0.5,
                              0.5, 0.0, 0.0, 0.1, 0.0,
                              0.1, 0.0, 0.0, 0.0, 0.5};

double desenho_a_2_b[25] =   {0.2, 0.2, 0.2, 0.0, 0.1,
                              0.0, 0.2, 0.2, 0.0, 0.0, 
                              0.0, 0.0, 0.1, 0.2, 0.2,
                              0.2, 0.0, 0.0, 0.1, 0.0,
                              0.1, 0.0, 0.0, 0.0, 0.2};

//Vetores para criar desenho 03
double desenho_a_3_r[25] =   {0.0, 0.0, 0.5, 0.0, 0.0,
                              0.0, 0.5, 0.5, 0.5, 0.0, 
                              0.5, 0.5, 0.0, 0.5, 0.5,
                              0.0, 0.5, 0.5, 0.5, 0.0,
                              0.0, 0.0, 0.5, 0.0, 0.0};

double desenho_a_3_g[25] =   {0.0, 0.0, 0.1, 0.0, 0.0,
                              0.0, 0.1, 0.1, 0.1, 0.0, 
                              0.1, 0.1, 0.0, 0.1, 0.1,
                              0.0, 0.1, 0.1, 0.1, 0.0,
                              0.0, 0.0, 0.1, 0.0, 0.0};

double desenho_a_3_b[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0, 
                              0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0};

//Vetores para criar desenho 04 
double desenho_a_4_r[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0, 
                              0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0};

double desenho_a_4_g[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0, 
                              0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0};
                        
double desenho_a_4_b[25] =   {0.0, 0.0, 0.1, 0.0, 0.0,
                              0.0, 0.1, 0.1, 0.1, 0.0, 
                              0.1, 0.1, 0.1, 0.1, 0.1,
                              0.0, 0.0, 0.1, 0.0, 0.0,
                              0.0, 0.0, 0.1, 0.0, 0.0};


//Vetores para criar desenho 05 
double desenho_a_5_r[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0, 
                              0.0, 0.0, 0.1, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0};

double desenho_a_5_g[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.1, 0.0, 0.0, 
                              0.0, 0.1, 0.1, 0.1, 0.0,
                              0.1, 0.1, 0.1, 0.1, 0.1,
                              0.0, 0.0, 0.0, 0.0, 0.0};

double desenho_a_5_b[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0, 
                              0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.0};

//ANIMAÇÃO B
//Vetores para criar desenho 01

double desenho_b_1_r[25] = 
{
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.4, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.4, 0.0, 0.4, 0.0, 0.0
};

double desenho_b_1_g[25] = 
{
    0.0, 0.0, 0.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 0.0, 0.0,
    0.26, 1.0, 1.0, 1.0, 0.0,
    0.0, 0.0, 1.0, 0.5, 0.0,
    0.26, 0.0, 0.26, 0.0, 0.0
};

double desenho_b_1_b[25] = 
{
    0.0, 0.0, 0.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 0.0, 0.0,
    0.13, 1.0, 1.0, 1.0, 0.0,
    0.0, 0.0, 1.0, 0.5, 0.0,
    0.13, 0.0, 0.13, 0.0, 0.0
};

//Vetores para criar desenho 02

double desenho_b_2_r[25] = 
{
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.4, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.4, 0.0, 0.0,
    0.4, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_2_g[25] = 
{
    0.0, 0.0, 0.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 0.0, 0.0,
    0.0, 0.26, 1.0, 1.0, 0.0,
    0.0, 0.0, 0.26, 0.5, 0.0,
    0.26, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_2_b[25] = 
{
    0.0, 0.0, 0.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 0.0, 0.0,
    0.0, 0.13, 1.0, 1.0, 0.0,
    0.0, 0.0, 0.13, 0.5, 0.0,
    0.13, 0.0, 0.0, 0.0, 0.0
};

//Vetores para criar desenho 03

double desenho_b_3_r[25] = 
{
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.4, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.4, 0.0, 0.4, 0.0,
    0.4, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_3_g[25] = 
{
    0.0, 0.0, 0.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 0.26, 0.0,
    0.0, 0.0, 0.5, 1.0, 0.0,
    0.0, 0.26, 0.0, 0.26, 0.0,
    0.26, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_3_b[25] = 
{
    0.0, 0.0, 0.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 0.13, 0.0,
    0.0, 0.0, 0.5, 1.0, 0.0,
    0.0, 0.13, 0.0, 0.13, 0.0,
    0.13, 0.0, 0.0, 0.0, 0.0
};
//Vetores para criar desenho 04

double desenho_b_4_r[25] = 
{
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.4, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.4, 0.0,
    0.0, 0.0, 0.0, 0.4, 0.0,
    0.4, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_4_g[25] = 
{
    0.0, 0.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 0.26, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.26, 1.0,
    0.0, 0.0, 0.0, 0.26, 0.0,
    0.26, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_4_b[25] = 
{
    0.0, 0.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 0.13, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.13, 1.0,
    0.0, 0.0, 0.0, 0.13, 0.0,
    0.13, 0.0, 0.0, 0.0, 0.0
};

//Vetores para criar desenho 05

double desenho_b_5_r[25] = 
{
    0.0, 0.0, 0.0, 0.4, 0.0,
    0.4, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.4, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.4, 0.0,
    0.4, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_5_g[25] = 
{
    0.0, 0.0, 1.0, 0.26, 0.0,
    0.26, 0.5, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.26, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.26, 0.0,
    0.26, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_5_b[25] = 
{
    0.0, 0.0, 1.0, 0.13, 0.0,
    0.13, 0.5, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.13, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.13, 0.0,
    0.13, 0.0, 0.0, 0.0, 0.0
};

//Vetores para criar desenho 06

double desenho_b_6_r[25] = 
{
    0.0, 0.0, 0.0, 0.0, 0.4,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.4, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.4, 0.0,
    0.4, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_6_g[25] = 
{
    0.0, 0.0, 1.0, 0.0, 0.26,
    0.0, 0.5, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.26, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.26, 0.0,
    0.26, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_6_b[25] = 
{
    0.0, 0.0, 1.0, 0.0, 0.13,
    0.0, 0.5, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.13, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.13, 0.0,
    0.13, 0.0, 0.0, 0.0, 0.0
};

//Vetores para criar desenho 07

double desenho_b_7_r[25] = 
{
    0.0, 0.0, 0.0, 0.0, 0.4,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.4, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.4, 0.0,
    0.4, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_7_g[25] = 
{
    0.0, 1.0, 1.0, 0.0, 0.26,
    0.0, 0.5, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.26, 1.0, 1.0,
    1.0, 0.0, 0.0, 0.26, 0.0,
    0.26, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_7_b[25] = {
    0.0, 1.0, 1.0, 0.0, 0.13,
    0.0, 0.5, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.13, 1.0, 1.0,
    1.0, 0.0, 0.0, 0.13, 0.0,
    0.13, 0.0, 0.0, 0.0, 0.0
};

//Vetores para criar desenho 08

double desenho_b_8_r[25] = 
{
    0.0, 0.0, 0.0, 0.0, 0.4,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.4, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.4, 0.0,
    0.4, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_8_g[25] = 
{
    0.0, 1.0, 1.0, 0.0, 0.26,
    1.0, 0.5, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.26, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.26, 0.0,
    0.26, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_8_b[25] = 
{
    0.0, 1.0, 1.0, 0.0, 0.13,
    1.0, 0.5, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.13, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.13, 0.0,
    0.13, 0.0, 0.0, 0.0, 0.0
};

//Vetores para criar desenho 09

double desenho_b_9_r[25] = 
{
    0.0, 0.0, 0.0, 0.0, 0.4,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.4, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.4, 0.0,
    0.4, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_9_g[25] = 
{
    0.0, 1.0, 1.0, 0.0, 0.26,
    1.0, 0.5, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.26, 1.0, 1.0,
    0.0, 1.0, 0.0, 0.26, 0.0,
    0.26, 0.0, 0.0, 0.0, 0.0
};

double desenho_b_9_b[25] = 
{
    0.0, 1.0, 1.0, 0.0, 0.13,
    1.0, 0.5, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.13, 1.0, 1.0,
    0.0, 1.0, 0.0, 0.13, 0.0,
    0.13, 0.0, 0.0, 0.0, 0.0
};



//imprimir valor binário
void imprimir_binario(int num) {
 int i;
 for (i = 31; i >= 0; i--) {
  (num & (1 << i)) ? printf("1") : printf("0");
 }
}

//rotina para definição da intensidade de cores do led
uint32_t matrix_rgb(double b, double r, double g)
{
  unsigned char R, G, B;
  R = r * 255;
  G = g * 255;
  B = b * 255;
  return (G << 24) | (R << 16) | (B << 8);
}

//rotina para acionar a matrix de leds - ws2812b
void desenho_pio(double *desenho_r, double *desenho_g, double *desenho_b, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b){

    for (int16_t i = 0; i < NUM_PIXELS; i++) {

        valor_led = matrix_rgb(desenho_b[24-i], desenho_r[24-i], desenho_g[24-i]);

        // Envia o valor para a PIO
        pio_sm_put_blocking(pio, sm, valor_led);
    }

}

//rotina da interrupção
static void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (current_time - last_time > 200000) {
        last_time = current_time;

        if (gpio == button_0) {
            printf("Bounce A = %d\n", a);
            printf("Habilitacao da Animacao A = %d\n", a);
            p_flag_1 = true;
            a++;
        } else if (gpio == button_1) {
            printf("Bounce B = %d\n", ab);
            printf("Habilitacao da Animacao B = %d\n", ab);
            p_flag_2 = true;
            ab++;
        }
    }
}

//função principal
int main()
{
    //coloca a frequência de clock para 128 MHz, facilitando a divisão pelo clock
    ok = set_sys_clock_khz(128000, false);

    // Inicializa todos os códigos stdio padrão que estão ligados ao binário.
    stdio_init_all();

    printf("iniciando a transmissão PIO");
    if (ok) printf("clock set to %ld\n", clock_get_hz(clk_sys));

    //configurações da PIO
    uint offset = pio_add_program(pio, &pio_matrix_program);
    sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    //inicializar o botão de interrupção - GPIO5
    gpio_init(button_0);
    gpio_set_dir(button_0, GPIO_IN);
    gpio_pull_up(button_0);

    //inicializar o botão de interrupção - GPIO6
    gpio_init(button_1);
    gpio_set_dir(button_1, GPIO_IN);
    gpio_pull_up(button_1);


    //interrupção da gpio habilitada

    gpio_set_irq_enabled_with_callback(button_0, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(button_1, GPIO_IRQ_EDGE_FALL, true);


    while (true) {
    sleep_ms(500);
    printf("\nfrequeência de clock %ld\r\n", clock_get_hz(clk_sys));
    desenho_pio(desenho_0, desenho_0, desenho_0, valor_led, pio, sm, r, g, b);

    // Verifica se a flag de animação A está ativa
    if (p_flag_1 == 1)
    {
        sleep_ms(1000);
        printf("Animacao A\n");
        desenho_pio(desenho_a_1_r, desenho_a_1_g, desenho_a_1_b, valor_led, pio, sm, r, g, b);
        sleep_ms(1000);

        desenho_pio(desenho_a_2_r, desenho_a_2_g, desenho_a_2_b, valor_led, pio, sm, r, g, b);
        sleep_ms(1000);

        desenho_pio(desenho_a_3_r, desenho_a_3_g, desenho_a_3_b, valor_led, pio, sm, r, g, b);
        sleep_ms(1000);

        desenho_pio(desenho_a_4_r, desenho_a_4_g, desenho_a_4_b, valor_led, pio, sm, r, g, b);
        sleep_ms(1000);

        desenho_pio(desenho_a_5_r, desenho_a_5_g, desenho_a_5_b, valor_led, pio, sm, r, g, b);
        sleep_ms(1000);

        p_flag_1 = 0;
    }

    // Verifica se a flag de animação B está ativa
    else if (p_flag_2 == 1)
    {
    sleep_ms(1000);
    printf("Animacao B\n");

    desenho_pio(desenho_b_1_r, desenho_b_1_g, desenho_b_1_b, valor_led, pio, sm, r, g, b);
    sleep_ms(1000);

    desenho_pio(desenho_b_2_r, desenho_b_2_g, desenho_b_2_b, valor_led, pio, sm, r, g, b);
    sleep_ms(1000);

    desenho_pio(desenho_b_3_r, desenho_b_3_g, desenho_b_3_b, valor_led, pio, sm, r, g, b);
    sleep_ms(1000);

    desenho_pio(desenho_b_4_r, desenho_b_4_g, desenho_b_4_b, valor_led, pio, sm, r, g, b);
    sleep_ms(1000);

    desenho_pio(desenho_b_5_r, desenho_b_5_g, desenho_b_5_b, valor_led, pio, sm, r, g, b);
    sleep_ms(1000);

    desenho_pio(desenho_b_6_r, desenho_b_6_g, desenho_b_6_b, valor_led, pio, sm, r, g, b);
    sleep_ms(1000);

    desenho_pio(desenho_b_7_r, desenho_b_7_g, desenho_b_7_b, valor_led, pio, sm, r, g, b);
    sleep_ms(1000);

    desenho_pio(desenho_b_8_r, desenho_b_8_g, desenho_b_8_b, valor_led, pio, sm, r, g, b);
    sleep_ms(1000);

    desenho_pio(desenho_b_9_r, desenho_b_9_g, desenho_b_9_b, valor_led, pio, sm, r, g, b);
    sleep_ms(1000);

    p_flag_2 = 0;
    }

    
    }
    
}