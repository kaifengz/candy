
TARGET = constr

all: $(TARGET)

check: $(TARGET)
	./$(TARGET)

$(TARGET): range.ml constr.ml
	ocamlc -o $@ $^
	-rm -f *.cm*

clean:
	-rm -f constr *.cm*

