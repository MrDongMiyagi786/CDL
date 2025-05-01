
//handwritten lexer using java
import java.util.Arrays;
import java.util.List;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class Lexer {
    // Token types
    public enum TokenType {
        IDENTIFIER,
        NUMBER,
        OPERATOR,
        KEYWORD,
        STRING,
        SYMBOL,
        EOF,
        ERROR
    }

    // Token class
    public static class Token {
        public final TokenType type;
        public final String value;
        public final int line;
        public final int column;

        public Token(TokenType type, String value, int line, int column) {
            this.type = type;
            this.value = value;
            this.line = line;
            this.column = column;
        }

        @Override
        public String toString() {
            return String.format("Token: %-12s Value: %-10s Line: %d, Column: %d",
                    type, value, line, column);
        }
    }

    // Keywords
    private static final List<String> KEYWORDS = Arrays.asList(
            "if", "else", "while", "for", "int", "float", "char", "return", "void", "main");

    // Input text and position tracking
    private final String input;
    private int position;
    private int line;
    private int column;

    public Lexer(String input) {
        this.input = input;
        this.position = 0;
        this.line = 1;
        this.column = 1;
    }

    // Check if a string is a keyword
    private boolean isKeyword(String str) {
        return KEYWORDS.contains(str);
    }

    // Get the next token
    public Token getNextToken() {
        // Skip whitespace
        while (position < input.length() && Character.isWhitespace(input.charAt(position))) {
            if (input.charAt(position) == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            position++;
        }

        // Check for end of input
        if (position >= input.length()) {
            return new Token(TokenType.EOF, "EOF", line, column);
        }

        char current = input.charAt(position);

        // Handle identifiers and keywords
        if (Character.isLetter(current) || current == '_') {
            StringBuilder buffer = new StringBuilder();
            while (position < input.length() &&
                    (Character.isLetterOrDigit(input.charAt(position)) ||
                            input.charAt(position) == '_')) {
                buffer.append(input.charAt(position));
                position++;
                column++;
            }

            String value = buffer.toString();
            return new Token(
                    isKeyword(value) ? TokenType.KEYWORD : TokenType.IDENTIFIER,
                    value,
                    line,
                    column - value.length());
        }

        // Handle numbers
        if (Character.isDigit(current)) {
            StringBuilder buffer = new StringBuilder();
            while (position < input.length() &&
                    (Character.isDigit(input.charAt(position)) ||
                            input.charAt(position) == '.')) {
                buffer.append(input.charAt(position));
                position++;
                column++;
            }
            return new Token(TokenType.NUMBER, buffer.toString(), line, column - buffer.length());
        }

        // Handle strings
        if (current == '"') {
            StringBuilder buffer = new StringBuilder();
            position++;
            column++;
            while (position < input.length() && input.charAt(position) != '"') {
                buffer.append(input.charAt(position));
                position++;
                column++;
            }
            if (position < input.length() && input.charAt(position) == '"') {
                position++;
                column++;
                return new Token(TokenType.STRING, buffer.toString(), line, column - buffer.length() - 2);
            } else {
                return new Token(TokenType.ERROR, "Unterminated string", line, column);
            }
        }

        // Handle operators and symbols
        if ("+-*/=<>!&|".indexOf(current) != -1) {
            StringBuilder buffer = new StringBuilder();
            buffer.append(current);
            position++;
            column++;

            // Check for two-character operators
            if (position < input.length() && "=<>|&".indexOf(input.charAt(position)) != -1) {
                buffer.append(input.charAt(position));
                position++;
                column++;
            }

            return new Token(TokenType.OPERATOR, buffer.toString(), line, column - buffer.length());
        }

        // Handle single-character symbols
        if ("(){}[];,:. ".indexOf(current) != -1) {
            String value = String.valueOf(current);
            position++;
            column++;
            return new Token(TokenType.SYMBOL, value, line, column - 1);
        }

        // Unknown character
        String value = String.valueOf(current);
        position++;
        column++;
        return new Token(TokenType.ERROR, value, line, column - 1);
    }

    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Usage: java Lexer <input_file>");
            System.exit(1);
        }

        try {
            // Read the entire file
            StringBuilder inputBuilder = new StringBuilder();
            try (BufferedReader reader = new BufferedReader(new FileReader(args[0]))) {
                String line;
                while ((line = reader.readLine()) != null) {
                    inputBuilder.append(line).append("\n");
                }
            }

            String input = inputBuilder.toString();
            Lexer lexer = new Lexer(input);
            System.out.println("Lexical Analysis Results:");
            System.out.println("------------------------");

            Token token;
            do {
                token = lexer.getNextToken();
                System.out.println(token);
            } while (token.type != TokenType.EOF && token.type != TokenType.ERROR);
        } catch (IOException e) {
            System.out.println("Error reading file: " + e.getMessage());
            System.exit(1);
        }
    }
}
// Note to run :name the code Lexer 1]javac Lexer.java then 2] java Lexer
// test_input.txt
// int main() {
// int x = 10;
// if (x > 5) {
// printf("Hello, world!");
// }
// return 0;
// }
