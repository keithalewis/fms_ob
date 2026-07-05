import csv
import sqlite3
from pathlib import Path
from datetime import date, datetime, timedelta

def load_lobster_csv(csv_path, db_path, table_name, symbol, trading_date):
    """
    trading_date: a datetime.date object, e.g. date(2024, 1, 5)
    """

    csv_path = Path(csv_path)
    db_path = Path(db_path)

    conn = sqlite3.connect(db_path)
    cur = conn.cursor()

    schema = f"""
        CREATE TABLE IF NOT EXISTS {table_name} (
            order_id   INTEGER,
            symbol     TEXT,
            type       INTEGER,
            ts         TEXT,
            level      INTEGER,
            size       INTEGER,
            entity     TEXT
        );
    """
    cur.execute(schema)
    conn.commit()

    # 34200.005929046,1,16114346,1300,1356500,-1
    def parse_row(row):
        ts_raw = float(row[0])
        dt = datetime.combine(trading_date, datetime.min.time()) \
             + timedelta(seconds=ts_raw)
        return (row[2], symbol, row[1], dt.isoformat(), int(row[4]), int(row[3])*int(row[5]), "exchange")

    placeholders = "?,?,?,?,?,?,?"
    insert_sql = f"INSERT INTO {table_name} VALUES ({placeholders})"

    # Bulk insert
    with csv_path.open("r", newline="") as f:
        reader = csv.reader(f)
        conn.execute("BEGIN")
        for row in reader:
            cur.execute(insert_sql, parse_row(row))
            #print(parse_row(row))
        conn.commit()

    conn.close()
    print(f"Loaded {csv_path} into table '{table_name}' with real timestamps.")

if __name__ == "__main__":
	csv_path = "SPY_2012-06-21_34200000_37800000_message_50.csv"
	symbol = "SPY"
	db_path = "./lobster.db"
	table_name = "message"
	trading_date = datetime(2012,6,21)
	load_lobster_csv(csv_path, db_path, table_name, symbol, trading_date)

"""
:!python3 %
"""
