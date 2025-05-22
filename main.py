import pandas as pd
import tkinter as tk
from tkinter import filedialog, messagebox

GPL_TEXT = """\
GNU GENERAL PUBLIC LICENSE
Version 3, 29 June 2007

Copyright (C) 2025 Moritz Breier

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Full license: https://www.gnu.org/licenses/gpl-3.0.html
"""

def transform_csv(input_path, output_path):
    try:
        df = pd.read_csv(input_path, sep=";", dtype=str).fillna("")

        # Neue Zeilen
        new_rows = []

        # Neue Kopfzeile: feste Struktur plus restliche Spalten ab Index 6
        new_header = ["Vorname", "Nachname", "Adresse"] + df.columns[6:].tolist()
        new_rows.append(new_header)

        for i in range(1, len(df)):
            row = df.iloc[i]
            shared_data = row[6:].tolist()

            # Person 1
            if row[0].strip():
                person1 = [row[0], row[1], row[4]] + shared_data
                new_rows.append(person1)

            # Person 2 (nur wenn Vorname2 vorhanden)
            if row[2].strip():
                person2 = [row[2], row[3], row[5]] + shared_data
                new_rows.append(person2)

        # Neue CSV schreiben
        new_df = pd.DataFrame(new_rows)
        new_df.to_csv(output_path, sep=";", index=False, header=False, encoding="utf-8-sig")
        messagebox.showinfo("Erfolg", "Datei erfolgreich verarbeitet.")
    except Exception as e:
        messagebox.showerror("Fehler", f"Ein Fehler ist aufgetreten:\n{e}")

def show_license():
    messagebox.showinfo("Lizenz – GPLv3", GPL_TEXT)

def open_gui():
    def select_input_file():
        path = filedialog.askopenfilename(title="Eingabedatei wählen", filetypes=[("CSV-Dateien", "*.csv")])
        if path:
            input_entry.delete(0, tk.END)
            input_entry.insert(0, path)

    def select_output_file():
        path = filedialog.asksaveasfilename(title="Ausgabedatei wählen", defaultextension=".csv", filetypes=[("CSV-Dateien", "*.csv")])
        if path:
            output_entry.delete(0, tk.END)
            output_entry.insert(0, path)

    def run_transform():
        input_path = input_entry.get()
        output_path = output_entry.get()
        if not input_path or not output_path:
            messagebox.showwarning("Fehlende Angaben", "Bitte Eingabe- und Ausgabedatei angeben.")
            return
        transform_csv(input_path, output_path)

    # GUI erstellen
    root = tk.Tk()
    root.title("CSV IServ Converter")

    tk.Label(root, text="Eingabedatei:").grid(row=0, column=0, sticky="w", padx=5, pady=5)
    input_entry = tk.Entry(root, width=50)
    input_entry.grid(row=0, column=1, padx=5)
    tk.Button(root, text="Durchsuchen", command=select_input_file).grid(row=0, column=2, padx=5)

    tk.Label(root, text="Ausgabedatei:").grid(row=1, column=0, sticky="w", padx=5, pady=5)
    output_entry = tk.Entry(root, width=50)
    output_entry.grid(row=1, column=1, padx=5)
    tk.Button(root, text="Speichern unter", command=select_output_file).grid(row=1, column=2, padx=5)

    tk.Button(root, text="Start", command=run_transform, width=20).grid(row=2, column=0, columnspan=3, pady=15)
    # Lizenz-Button unten links
    tk.Button(root, text="Lizenz anzeigen", command=show_license).grid(row=3, column=0, sticky="w", padx=10, pady=(0, 5))

    # Copyright unten rechts
    tk.Label(root, text="© 2025 Moritz Breier", font=("Arial", 8), anchor="e").grid(
        row=3, column=2, sticky="e", padx=10, pady=(0, 5)
    )


    root.mainloop()


if __name__ == "__main__":
    open_gui()
