CC = gcc
CFLAGS = -Wall -Wextra -Ihexagon
TARGET = hexo
SRCDIR = hexagon
OBJDIR = build

# List of source files
SRCS = $(wildcard $(SRCDIR)/*.c)
# Transform SRCS list to OBJS list in the build directory
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# Default rule
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create the build directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean up
clean:
	rm -rf $(OBJDIR)/* $(TARGET)

.PHONY: all clean