# pico-owo
OwO language, a dialect of Lisp, for Pico Pi.

### Compile
```bash
export PICO_SDK_PATH=/PATH/TO/pico-sdk
cmake ..
make
```

### Blink LED
```
def {LED_PIN} 25
gpio_init LED_PIN
gpio_set_dir "out" LED_PIN
def {nil} {}
def {fst} [\ {list} {eval [head list]}]
def {map} [\ {func args} {if [== args nil] {nil} {join [list [func [fst args]]] [map func [tail args]]} }]
def {blink} [\ {interval} {map eval [list {gpio_put 1 LED_PIN} {sleep_ms interval} {gpio_put 0 LED_PIN} {sleep_ms interval} {print "blink"}]}]
loop 1 {blink 500}
```
