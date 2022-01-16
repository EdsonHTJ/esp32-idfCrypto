#ifndef __PRODUCTS__
#define __PRODUCTS__

#define RELAY1 12
#define RELAY2 14
#define RELAY3 27
#define RELAY4 26

typedef struct product
{
    char *productName;
    float price;
}product_t;

const product_t PRODUCT1 = {
    .productName = "Capuccino",
    .price = 1.2,
};

const product_t PRODUCT2 = {
    .productName = "Latte",
    .price = 1.5,
};

const product_t PRODUCT3 = {
    .productName = "Chocolate",
    .price = 2,
};

const product_t PRODUCT4 = {
    .productName = "Chocolate + Capuccino",
    .price = 2.2,
};

product_t products[] = {PRODUCT1, PRODUCT2, PRODUCT3, PRODUCT4};
const int PRODUCT_LIST_SIZE = sizeof(products) / sizeof(product_t);

product_t* get_product_list() {
    return products;
}





#endif 