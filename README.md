## ER-kaavio

```mermaid
erDiagram
    BOARD {
        int row PK
        int column PK
    }
    CELL {
        int row FK
        int column FK
        int current
        int next
    }
    BOARD ||--o{ CELL : "sisältää"
