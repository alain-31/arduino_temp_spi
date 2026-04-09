#include "stm32f411xe.h"
#include <stdint.h>

#define CS_LOW()   (GPIOA->BSRR = (1U << (4 + 16)))  // reset PA4
#define CS_HIGH()  (GPIOA->BSRR = (1U << 4))         // set PA4

#define LED_BLUE 0
#define LED_GREEN 1
#define LED_RED 10

typedef struct __attribute__((packed)) {
    int16_t ax;
    int16_t ay;
    int16_t az;
    float gx;
    float gy;
    float gz;
    float temp;
} SensorData;

static void delay(volatile uint32_t count)
{
    while (count--) {
        __asm volatile ("nop");
    }
}

static void gpio_init(void)
{
    // Enable GPIOA, GPIOB  and GPIOC clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

    // -------- SPI1 pins --------
    // PA5 = SCK  -> AF5
    // PA6 = MISO -> AF5
    // PA7 = MOSI -> AF5

    // Alternate function mode for PA5, PA6, PA7
    GPIOA->MODER &= ~((3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (7 * 2)));
    GPIOA->MODER |=  ((2U << (5 * 2)) | (2U << (6 * 2)) | (2U << (7 * 2)));

    // High speed for PA5, PA6, PA7
    GPIOA->OSPEEDR |= ((3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (7 * 2)));

    // No pull-up/pull-down
    GPIOA->PUPDR &= ~((3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (7 * 2)));

    // AF5 for SPI1 on PA5/PA6/PA7
    GPIOA->AFR[0] &= ~((0xFU << (5 * 4)) | (0xFU << (6 * 4)) | (0xFU << (7 * 4)));
    GPIOA->AFR[0] |=  ((5U   << (5 * 4)) | (5U   << (6 * 4)) | (5U   << (7 * 4)));

    // -------- PA4 as manual chip select --------
    GPIOA->MODER &= ~(3U << (4 * 2));
    GPIOA->MODER |=  (1U << (4 * 2));   // output
    GPIOA->OTYPER &= ~(1U << 4);        // push-pull
    GPIOA->OSPEEDR |= (3U << (4 * 2));  // high speed
    GPIOA->PUPDR &= ~(3U << (4 * 2));   // no pull
    CS_HIGH();


    // -------- PB0, PB1, PB10 to control the LEDs --------
    // output mode
    GPIOB->MODER &= ~(
            (3U << (LED_BLUE * 2)) |
            (3U << (LED_GREEN * 2)) |
            (3U << (LED_RED * 2))
        );

    GPIOB->MODER |= (
            (1U << (LED_BLUE * 2)) |
            (1U << (LED_GREEN * 2)) |
            (1U << (LED_RED * 2))
        );

    // Push-pull
    GPIOB->OTYPER &= ~(
            (1U << LED_BLUE) |
            (1U << LED_GREEN) |
            (1U << LED_RED)
        );

    // High speed (optionnel)
    GPIOB->OSPEEDR |= (
            (3U << (LED_BLUE * 2)) |
            (3U << (LED_GREEN * 2)) |
            (3U << (LED_RED * 2))
        );

    // No pull-up/down
    GPIOB->PUPDR &= ~(
            (3U << (LED_BLUE * 2)) |
            (3U << (LED_GREEN * 2)) |
            (3U << (LED_RED * 2))
        );
}

static void spi1_init(void)
{
    // Enable SPI1 clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Disable SPI before config
    SPI1->CR1 = 0;
    SPI1->CR2 = 0;

    // Master mode
    SPI1->CR1 |= SPI_CR1_MSTR;

    // Baud rate = fPCLK / 256  (slow for first tests)
    SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;

    // SPI mode 0: CPOL=0, CPHA=0
    SPI1->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);

    // Software slave management
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;

    // MSB first, 8-bit frame by default, full duplex
    SPI1->CR1 &= ~(SPI_CR1_LSBFIRST | SPI_CR1_DFF | SPI_CR1_BIDIMODE | SPI_CR1_RXONLY);

    // Enable SPI
    SPI1->CR1 |= SPI_CR1_SPE;
}

static uint8_t spi1_transfer(uint8_t data)
{
    // Wait until TX buffer empty
    while (!(SPI1->SR & SPI_SR_TXE)) {
    }

    *((volatile uint8_t *)&SPI1->DR) = data;

    // Wait until RX buffer not empty
    while (!(SPI1->SR & SPI_SR_RXNE)) {
    }

    uint8_t received = *((volatile uint8_t *)&SPI1->DR);

    // Wait until SPI not busy
    while (SPI1->SR & SPI_SR_BSY) {
    }

    return received;
}

static void spi_read_buffer(uint8_t *rx, uint32_t len)
{
    CS_LOW();
    delay(200);

    for (uint32_t i = 0; i < len; i++) {
        rx[i] = spi1_transfer(0x00);
    }

    delay(200);
    CS_HIGH();
}

int main(void)
{
    SensorData data;

    gpio_init();
    spi1_init();

    while (1) {
        spi_read_buffer((uint8_t *)&data, sizeof(SensorData));

        // BSSR will modify ODR (outdata register)
        if (data.temp<0) {
            GPIOB->BSRR = (1U << (LED_BLUE)); // led on 
            GPIOB->BSRR = (1U << (LED_GREEN + 16)); // led off
            GPIOB->BSRR = (1U << (LED_RED + 16));
        }
        else if (data.temp<30) {
            GPIOB->BSRR = (1U << (LED_BLUE + 16));
            GPIOB->BSRR = (1U << (LED_GREEN));
            GPIOB->BSRR = (1U << (LED_RED + 16));
        }
        else  {
            GPIOB->BSRR = (1U << (LED_BLUE + 16));
            GPIOB->BSRR = (1U << (LED_GREEN  + 16));
            GPIOB->BSRR = (1U << (LED_RED));
        }
        delay(2000000);
    }
}