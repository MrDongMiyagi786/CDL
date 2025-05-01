//optimization in c++
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>

using namespace std;

const int MAX_QUADS = 500;
const int MAX_STR = 100;

class Quadruple {
public:
    string op;
    string arg1;
    string arg2;
    string result;
    bool eliminated;
    bool is_leader;

    Quadruple() : eliminated(false), is_leader(false) {}
};

class QuadrupleOptimizer {
private:
    vector<Quadruple> quad_list;

    bool isConstant(const string& s) {
        if(s.empty()) return false;
        size_t start = (s[0] == '+' || s[0] == '-') ? 1 : 0;
        if(start == s.length()) return false;
        return s.find_first_not_of("0123456789", start) == string::npos;
    }

    int evaluateExpression(const string& op, int c1, int c2) {
        if(op == "+") return c1 + c2;
        if(op == "-") return c1 - c2;
        if(op == "*") return c1 * c2;
        if(op == "/") return c2 != 0 ? c1 / c2 : 0;
        return 0;
    }

public:
    bool readQuadsFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening Quad input file" << endl;
            return false;
        }
        cout << "Reading Quads from " << filename << "..." << endl;

        string line;
        while (getline(file, line) && quad_list.size() < MAX_QUADS) {
            if(line.empty()) continue;

            istringstream iss(line);
            Quadruple quad;
            iss >> quad.op;
            iss >> quad.arg1;
            iss >> quad.arg2;
            iss >> quad.result;

            if(quad.arg1 == "-") quad.arg1 = "";
            if(quad.arg2 == "-") quad.arg2 = "";
            if(quad.result == "-") quad.result = "";

            quad_list.push_back(quad);
        }

        file.close();
        cout << "Read " << quad_list.size() << " quads." << endl;
        return true;
    }

    void printQuads(const string& title, bool show_eliminated) {
        cout << "\n--- " << title << " ---" << endl;
        cout << "Nr | Op              | Arg1                      | Arg2                      | Result                    | " 
             << (show_eliminated ? "Status" : "") << endl;
        cout << "---|-----------------|---------------------------|---------------------------|---------------------------|--------" << endl;

        if (quad_list.empty()) {
            cout << "(No Quads)" << endl;
            return;
        }

        int displayed_count = 0;
        for (size_t i = 0; i < quad_list.size(); i++) {
            if (!quad_list[i].eliminated || show_eliminated) {
                displayed_count++;
                printf("%03d| %-15s | %-25s | %-25s | %-25s | %s\n", 
                       (int)i + 1,
                       quad_list[i].op.c_str(),
                       quad_list[i].arg1.c_str(),
                       quad_list[i].arg2.c_str(),
                       quad_list[i].result.c_str(),
                       show_eliminated ? (quad_list[i].eliminated ? "(Elim)" : "") : "");
            }
        }

        if (displayed_count == 0 && !show_eliminated) {
            cout << "(No quads remaining after optimization)" << endl;
        }
        cout << string(95, '-') << endl;
    }

    void commonSubexpressionElimination() {
        cout << "\nPerforming Common Subexpression Elimination..." << endl;
        bool eliminated_found = false;

        for (size_t i = 0; i < quad_list.size(); i++) {
            if (quad_list[i].eliminated) continue;
            if (quad_list[i].op == "=") continue;

            for (size_t j = 0; j < i; j++) {
                if (quad_list[j].eliminated) continue;
                if (quad_list[j].op == "=") continue;

                if (quad_list[i].op == quad_list[j].op &&
                    quad_list[i].arg1 == quad_list[j].arg1 &&
                    quad_list[i].arg2 == quad_list[j].arg2)
                {
                    cout << "  Found potential CSE: Quad " << i + 1 
                         << " (" << quad_list[i].op << " " << quad_list[i].arg1 << " " 
                         << quad_list[i].arg2 << " " << quad_list[i].result << ") same as Quad "
                         << j + 1 << " (" << quad_list[j].op << " " << quad_list[j].arg1 << " "
                         << quad_list[j].arg2 << " " << quad_list[j].result << ")" << endl;
                    cout << "    Eliminating Quad " << i + 1 << ". Uses of '" 
                         << quad_list[i].result << "' should be replaced by '" 
                         << quad_list[j].result << "'" << endl;

                    quad_list[i].eliminated = true;
                    eliminated_found = true;
                    break;
                }
            }
        }
        if (!eliminated_found) cout << "  No common subexpressions identified for elimination." << endl;
        cout << string(40, '-') << endl;
    }

    void constantFoldingOptimization() {
        cout << "\nPerforming Constant Folding..." << endl;
        bool changed = false;

        for (size_t i = 0; i < quad_list.size(); i++) {
            if ((quad_list[i].op == "+" || quad_list[i].op == "-" ||
                 quad_list[i].op == "*" || quad_list[i].op == "/") &&
                isConstant(quad_list[i].arg1) && isConstant(quad_list[i].arg2))
            {
                string orig_op = quad_list[i].op;
                string orig_arg1 = quad_list[i].arg1;
                string orig_arg2 = quad_list[i].arg2;

                int c1 = stoi(quad_list[i].arg1);
                int c2 = stoi(quad_list[i].arg2);

                if (quad_list[i].op == "/" && c2 == 0) continue;

                int result_val = evaluateExpression(quad_list[i].op, c1, c2);
                
                cout << "  Folding Quad " << i + 1 << " (" << quad_list[i].result 
                     << " = " << orig_arg1 << " " << orig_op << " " << orig_arg2 
                     << ") -> (" << quad_list[i].result << " = " << result_val << ")" << endl;

                quad_list[i].op = "=";
                quad_list[i].arg1 = to_string(result_val);
                quad_list[i].arg2 = "";
                changed = true;
            }
        }
        if (!changed) cout << "  No constant folding opportunities found." << endl;
        cout << string(40, '-') << endl;
    }

    void constantPropagationOptimization() {
        cout << "\nPerforming Constant Propagation (and Folding)..." << endl;
        bool changed_prop = false;
        bool changed_fold = false;

        // Constant Propagation
        for (size_t i = 0; i < quad_list.size(); i++) {
            if (quad_list[i].op == "=" && isConstant(quad_list[i].arg1) && quad_list[i].arg2.empty()) {
                string var_to_prop = quad_list[i].result;
                string const_val = quad_list[i].arg1;

                for (size_t j = i + 1; j < quad_list.size(); j++) {
                    bool propagated_here = false;
                    if ((quad_list[j].op == "+" || quad_list[j].op == "-" ||
                         quad_list[j].op == "*" || quad_list[j].op == "/")) {
                        
                        if (quad_list[j].arg1 == var_to_prop) {
                            quad_list[j].arg1 = const_val;
                            propagated_here = true;
                        }
                        if (quad_list[j].arg2 == var_to_prop) {
                            quad_list[j].arg2 = const_val;
                            propagated_here = true;
                        }
                    }
                    if (propagated_here) {
                        cout << "  Propagated '" << var_to_prop << " = " << const_val 
                             << "' from Quad " << i + 1 << " into Quad " << j + 1 << endl;
                        changed_prop = true;
                    }
                }
            }
        }
        if (!changed_prop) cout << "  No constant propagation opportunities found." << endl;

        // Follow-up Constant Folding
        cout << "  Running follow-up Constant Folding..." << endl;
        for (size_t i = 0; i < quad_list.size(); i++) {
            if ((quad_list[i].op == "+" || quad_list[i].op == "-" ||
                 quad_list[i].op == "*" || quad_list[i].op == "/") &&
                isConstant(quad_list[i].arg1) && isConstant(quad_list[i].arg2))
            {
                string orig_op = quad_list[i].op;
                string orig_arg1 = quad_list[i].arg1;
                string orig_arg2 = quad_list[i].arg2;

                int c1 = stoi(quad_list[i].arg1);
                int c2 = stoi(quad_list[i].arg2);

                if (quad_list[i].op == "/" && c2 == 0) continue;

                int result_val = evaluateExpression(quad_list[i].op, c1, c2);
                
                cout << "    Folding Quad " << i + 1 << " (" << quad_list[i].result 
                     << " = " << orig_arg1 << " " << orig_op << " " << orig_arg2 
                     << ") -> (" << quad_list[i].result << " = " << result_val << ")" << endl;

                quad_list[i].op = "=";
                quad_list[i].arg1 = to_string(result_val);
                quad_list[i].arg2 = "";
                changed_fold = true;
            }
        }
        if (!changed_fold) cout << "    No further folding opportunities found." << endl;
        cout << string(40, '-') << endl;
    }
};

int main(int argc, char* argv[]) {
    string quad_filename = "music_score1quads";
    if (argc > 1) {
        quad_filename = argv[1];
    }

    cout << "--- Quadruple Optimizer (C++ Version) ---" << endl;

    QuadrupleOptimizer optimizer;
    if (!optimizer.readQuadsFromFile(quad_filename)) {
        cerr << "Failed to read quadruples from " << quad_filename << ". Exiting." << endl;
        return 1;
    }

    optimizer.printQuads("Initial Quads Loaded", false);

    int option = 0;
    while (option < 1 || option > 3) {
        cout << "\nSelect optimization technique:" << endl;
        cout << "  1. Common Subexpression Elimination (CSE)" << endl;
        cout << "  2. Constant Folding" << endl;
        cout << "  3. Constant Propagation (includes Folding)" << endl;
        cout << "Enter your choice (1-3): ";
        cin >> option;

        if (cin.fail() || option < 1 || option > 3) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice. Please enter 1, 2, or 3." << endl;
            option = 0;
        }
    }

    switch(option) {
        case 1:
            optimizer.commonSubexpressionElimination();
            break;
        case 2:
            optimizer.constantFoldingOptimization();
            break;
        case 3:
            optimizer.constantPropagationOptimization();
            break;
    }

    optimizer.printQuads("Optimized Quadruples", false);
    return 0;
} 

// //optimization in java

// import java.io.*;
// import java.util.*;

// class Quadruple {
//     String op;
//     String arg1;
//     String arg2;
//     String result;
//     boolean eliminated;
//     boolean isLeader;

//     public Quadruple() {
//         eliminated = false;
//         isLeader = false;
//     }
// }

// public class Optimizer {
//     private static final int MAX_QUADS = 500;
//     private ArrayList<Quadruple> quadList;

//     public Optimizer() {
//         quadList = new ArrayList<>();
//     }

//     private boolean isConstant(String s) {
//         if (s == null || s.isEmpty()) return false;
//         int start = (s.charAt(0) == '+' || s.charAt(0) == '-') ? 1 : 0;
//         if (start == s.length()) return false;
//         for (int i = start; i < s.length(); i++) {
//             if (!Character.isDigit(s.charAt(i))) return false;
//         }
//         return true;
//     }

//     private int evaluateExpression(String op, int c1, int c2) {
//         switch (op) {
//             case "+": return c1 + c2;
//             case "-": return c1 - c2;
//             case "*": return c1 * c2;
//             case "/": return c2 != 0 ? c1 / c2 : 0;
//             default: return 0;
//         }
//     }

//     public boolean readQuadsFromFile(String filename) {
//         try (BufferedReader reader = new BufferedReader(new FileReader(filename))) {
//             System.out.println("Reading Quads from " + filename + "...");
//             String line;
//             while ((line = reader.readLine()) != null && quadList.size() < MAX_QUADS) {
//                 if (line.trim().isEmpty()) continue;

//                 String[] parts = line.trim().split("\\s+");
//                 if (parts.length >= 1) {
//                     Quadruple quad = new Quadruple();
//                     quad.op = parts[0];
//                     quad.arg1 = parts.length > 1 && !parts[1].equals("-") ? parts[1] : "";
//                     quad.arg2 = parts.length > 2 && !parts[2].equals("-") ? parts[2] : "";
//                     quad.result = parts.length > 3 && !parts[3].equals("-") ? parts[3] : "";
//                     quadList.add(quad);
//                 }
//             }
//             System.out.println("Read " + quadList.size() + " quads.");
//             return true;
//         } catch (IOException e) {
//             System.err.println("Error reading file: " + e.getMessage());
//             return false;
//         }
//     }

//     public void printQuads(String title, boolean showEliminated) {
//         System.out.println("\n--- " + title + " ---");
//         System.out.printf("%-3s| %-15s | %-25s | %-25s | %-25s | %s%n",
//                 "Nr", "Op", "Arg1", "Arg2", "Result", showEliminated ? "Status" : "");
//         System.out.println("-".repeat(95));

//         if (quadList.isEmpty()) {
//             System.out.println("(No Quads)");
//             return;
//         }

//         int displayedCount = 0;
//         for (int i = 0; i < quadList.size(); i++) {
//             Quadruple quad = quadList.get(i);
//             if (!quad.eliminated || showEliminated) {
//                 displayedCount++;
//                 System.out.printf("%03d| %-15s | %-25s | %-25s | %-25s | %s%n",
//                         i + 1, quad.op, quad.arg1, quad.arg2, quad.result,
//                         showEliminated ? (quad.eliminated ? "(Elim)" : "") : "");
//             }
//         }

//         if (displayedCount == 0 && !showEliminated) {
//             System.out.println("(No quads remaining after optimization)");
//         }
//         System.out.println("-".repeat(95));
//     }

//     public void commonSubexpressionElimination() {
//         System.out.println("\nPerforming Common Subexpression Elimination...");
//         boolean eliminatedFound = false;

//         for (int i = 0; i < quadList.size(); i++) {
//             if (quadList.get(i).eliminated) continue;
//             if (quadList.get(i).op.equals("=")) continue;

//             for (int j = 0; j < i; j++) {
//                 if (quadList.get(j).eliminated) continue;
//                 if (quadList.get(j).op.equals("=")) continue;

//                 Quadruple qi = quadList.get(i);
//                 Quadruple qj = quadList.get(j);

//                 if (qi.op.equals(qj.op) &&
//                     qi.arg1.equals(qj.arg1) &&
//                     qi.arg2.equals(qj.arg2))
//                 {
//                     System.out.printf("  Found potential CSE: Quad %d (%s %s %s %s) same as Quad %d (%s %s %s %s)%n",
//                             i + 1, qi.op, qi.arg1, qi.arg2, qi.result,
//                             j + 1, qj.op, qj.arg1, qj.arg2, qj.result);
//                     System.out.printf("    Eliminating Quad %d. Uses of '%s' should be replaced by '%s'%n",
//                             i + 1, qi.result, qj.result);

//                     qi.eliminated = true;
//                     eliminatedFound = true;
//                     break;
//                 }
//             }
//         }
//         if (!eliminatedFound) System.out.println("  No common subexpressions identified for elimination.");
//         System.out.println("-".repeat(40));
//     }

//     public void constantFoldingOptimization() {
//         System.out.println("\nPerforming Constant Folding...");
//         boolean changed = false;

//         for (int i = 0; i < quadList.size(); i++) {
//             Quadruple quad = quadList.get(i);
//             if ((quad.op.equals("+") || quad.op.equals("-") ||
//                  quad.op.equals("*") || quad.op.equals("/")) &&
//                 isConstant(quad.arg1) && isConstant(quad.arg2))
//             {
//                 String origOp = quad.op;
//                 String origArg1 = quad.arg1;
//                 String origArg2 = quad.arg2;

//                 int c1 = Integer.parseInt(quad.arg1);
//                 int c2 = Integer.parseInt(quad.arg2);

//                 if (quad.op.equals("/") && c2 == 0) continue;

//                 int resultVal = evaluateExpression(quad.op, c1, c2);
                
//                 System.out.printf("  Folding Quad %d (%s = %s %s %s) -> (%s = %d)%n",
//                         i + 1, quad.result, origArg1, origOp, origArg2, quad.result, resultVal);

//                 quad.op = "=";
//                 quad.arg1 = String.valueOf(resultVal);
//                 quad.arg2 = "";
//                 changed = true;
//             }
//         }
//         if (!changed) System.out.println("  No constant folding opportunities found.");
//         System.out.println("-".repeat(40));
//     }

//     public void constantPropagationOptimization() {
//         System.out.println("\nPerforming Constant Propagation (and Folding)...");
//         boolean changedProp = false;
//         boolean changedFold = false;

//         // Constant Propagation
//         for (int i = 0; i < quadList.size(); i++) {
//             Quadruple quad = quadList.get(i);
//             if (quad.op.equals("=") && isConstant(quad.arg1) && quad.arg2.isEmpty()) {
//                 String varToProp = quad.result;
//                 String constVal = quad.arg1;

//                 for (int j = i + 1; j < quadList.size(); j++) {
//                     Quadruple qj = quadList.get(j);
//                     boolean propagatedHere = false;

//                     if (qj.op.equals("+") || qj.op.equals("-") ||
//                         qj.op.equals("*") || qj.op.equals("/"))
//                     {
//                         if (qj.arg1.equals(varToProp)) {
//                             qj.arg1 = constVal;
//                             propagatedHere = true;
//                         }
//                         if (qj.arg2.equals(varToProp)) {
//                             qj.arg2 = constVal;
//                             propagatedHere = true;
//                         }
//                     }

//                     if (propagatedHere) {
//                         System.out.printf("  Propagated '%s = %s' from Quad %d into Quad %d%n",
//                                 varToProp, constVal, i + 1, j + 1);
//                         changedProp = true;
//                     }
//                 }
//             }
//         }
//         if (!changedProp) System.out.println("  No constant propagation opportunities found.");

//         // Follow-up Constant Folding
//         System.out.println("  Running follow-up Constant Folding...");
//         for (int i = 0; i < quadList.size(); i++) {
//             Quadruple quad = quadList.get(i);
//             if ((quad.op.equals("+") || quad.op.equals("-") ||
//                  quad.op.equals("*") || quad.op.equals("/")) &&
//                 isConstant(quad.arg1) && isConstant(quad.arg2))
//             {
//                 String origOp = quad.op;
//                 String origArg1 = quad.arg1;
//                 String origArg2 = quad.arg2;

//                 int c1 = Integer.parseInt(quad.arg1);
//                 int c2 = Integer.parseInt(quad.arg2);

//                 if (quad.op.equals("/") && c2 == 0) continue;

//                 int resultVal = evaluateExpression(quad.op, c1, c2);
                
//                 System.out.printf("    Folding Quad %d (%s = %s %s %s) -> (%s = %d)%n",
//                         i + 1, quad.result, origArg1, origOp, origArg2, quad.result, resultVal);

//                 quad.op = "=";
//                 quad.arg1 = String.valueOf(resultVal);
//                 quad.arg2 = "";
//                 changedFold = true;
//             }
//         }
//         if (!changedFold) System.out.println("    No further folding opportunities found.");
//         System.out.println("-".repeat(40));
//     }

//     public static void main(String[] args) {
//         String quadFilename = "music_score1quads";
//         if (args.length > 0) {
//             quadFilename = args[0];
//         }

//         System.out.println("--- Quadruple Optimizer (Java Version) ---");

//         Optimizer optimizer = new Optimizer();
//         if (!optimizer.readQuadsFromFile(quadFilename)) {
//             System.err.println("Failed to read quadruples from " + quadFilename + ". Exiting.");
//             System.exit(1);
//         }

//         optimizer.printQuads("Initial Quads Loaded", false);

//         Scanner scanner = new Scanner(System.in);
//         int option = 0;
//         while (option < 1 || option > 3) {
//             System.out.println("\nSelect optimization technique:");
//             System.out.println("  1. Common Subexpression Elimination (CSE)");
//             System.out.println("  2. Constant Folding");
//             System.out.println("  3. Constant Propagation (includes Folding)");
//             System.out.print("Enter your choice (1-3): ");
            
//             try {
//                 option = scanner.nextInt();
//                 if (option < 1 || option > 3) {
//                     System.out.println("Invalid choice. Please enter 1, 2, or 3.");
//                     option = 0;
//                 }
//             } catch (InputMismatchException e) {
//                 System.out.println("Invalid input. Please enter a number between 1 and 3.");
//                 scanner.nextLine();
//                 option = 0;
//             }
//         }

//         switch(option) {
//             case 1:
//                 optimizer.commonSubexpressionElimination();
//                 break;
//             case 2:
//                 optimizer.constantFoldingOptimization();
//                 break;
//             case 3:
//                 optimizer.constantPropagationOptimization();
//                 break;
//         }

//         optimizer.printQuads("Optimized Quadruples", false);
//     }
// }
// run code- 
// g++ quad_optimizer.cpp -o optimizer
// ./ optimizer input.tac