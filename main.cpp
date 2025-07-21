#include <QApplication>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QIcon>
#include <QPalette>

const QString GPL_TEXT = R"(GNU GENERAL PUBLIC LICENSE
Version 3, 29 June 2007
Copyright (C) 2025 Moritz Breier
This program is free software...
Full license: https://www.gnu.org/licenses/gpl-3.0.html)";

const QString NOTICE_TEXT = R"(Dieses Programm kann Fehler enthalten.
Die Output-Datei sollte deshalb immer geprüft werden.)";

void transform_csv(const QString &input_path, const QString &output_path) {
    QFile inFile(input_path);
    QFile outFile(output_path);

    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Fehler", "Konnte Eingabedatei nicht öffnen.");
        return;
    }
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Fehler", "Konnte Ausgabedatei nicht schreiben.");
        return;
    }

    QTextStream in(&inFile);
    QTextStream out(&outFile);
    QStringList headers;
    QList<QStringList> new_rows;

    bool firstLine = true;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList row = line.split(";");

        if (firstLine) {
            headers = row.mid(6);
            new_rows.append(QStringList{"Vorname", "Nachname", "Adresse"} + headers);
            firstLine = false;
            continue;
        }

        QStringList shared = row.mid(6);
        if (!row[0].trimmed().isEmpty()) {
            new_rows.append(QStringList{row[0], row[1], row[4]} + shared);
        }
        if (!row[2].trimmed().isEmpty()) {
            new_rows.append(QStringList{row[2], row[3], row[5]} + shared);
        }
    }

    for (const auto &row : new_rows) {
        out << row.join(";") << "\n";
    }

    QMessageBox::information(nullptr, "Erfolg", "Datei erfolgreich verarbeitet.");
}

void applyBlueButtonPalette(QPushButton *btn) {
    QColor blueAccent(0, 0, 255);
    QPalette p = btn->palette();
    p.setColor(QPalette::Button, blueAccent);
    p.setColor(QPalette::ButtonText, Qt::white);
    btn->setAutoFillBackground(true);
    btn->setPalette(p);
    btn->update();
}

QPalette createDarkPalette() {
    QColor blueAccent(0, 0, 255);
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(30, 30, 30));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(15, 15, 15));
    darkPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, blueAccent);
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, blueAccent);
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    return darkPalette;
}

void applyDarkStyle(QWidget *w) {
    w->setStyleSheet(R"(
        QWidget {
            background-color: #1e1e1e;
            color: white;
        }
        QPushButton {
            background-color: #0000ff;
            color: white;
            border-radius: 5px;
            padding: 6px;
        }
        QPushButton:hover {
            background-color: #3333ff;
        }
        QLineEdit, QTextEdit {
            background-color: #121212;
            color: white;
            border: 1px solid #0000ff;
            border-radius: 3px;
        }
        QCheckBox {
            color: white;
        }
        QLabel {
            color: white;
        }
    )");
}

void applyLightStyle(QWidget *w) {
    w->setStyleSheet(R"(
        QWidget {
            background-color: white;
            color: black;
        }
        QPushButton {
            background-color: #0000ff;
            color: white;
            border-radius: 5px;
            padding: 6px;
        }
        QPushButton:hover {
            background-color: #3333ff;
        }
        QLineEdit, QTextEdit {
            background-color: white;
            color: black;
            border: 1px solid #0000ff;
            border-radius: 3px;
        }
        QCheckBox {
            color: black;
        }
        QLabel {
            color: black;
        }
    )");
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("CSV IServ Converter");
    window.setFixedSize(600, 250);
    window.setWindowIcon(QIcon(":/csv.ico")); // Optional

    auto *layout = new QVBoxLayout();

    auto *darkModeToggle = new QCheckBox("Darkmode");
    layout->addWidget(darkModeToggle, 0, Qt::AlignRight);

    auto *inputLine = new QLineEdit();
    auto *outputLine = new QLineEdit();
    auto *browseInput = new QPushButton("Durchsuchen");
    auto *browseOutput = new QPushButton("Speichern unter");
    auto *startButton = new QPushButton("Start");

    auto *licenseBtn = new QPushButton("Lizenz anzeigen");
    auto *noticeBtn = new QPushButton("Achtung");

    auto *hlayout1 = new QHBoxLayout();
    hlayout1->addWidget(new QLabel("Eingabedatei:"));
    hlayout1->addWidget(inputLine);
    hlayout1->addWidget(browseInput);

    auto *hlayout2 = new QHBoxLayout();
    hlayout2->addWidget(new QLabel("Ausgabedatei:"));
    hlayout2->addWidget(outputLine);
    hlayout2->addWidget(browseOutput);

    auto *hlayout3 = new QHBoxLayout();
    hlayout3->addWidget(licenseBtn);
    hlayout3->addWidget(noticeBtn);
    hlayout3->addStretch();

    layout->addLayout(hlayout1);
    layout->addLayout(hlayout2);
    layout->addWidget(startButton);
    layout->addLayout(hlayout3);

    QLabel *copyright = new QLabel("© 2025 Moritz Breier");
    layout->addWidget(copyright, 0, Qt::AlignRight);

    window.setLayout(layout);

    applyBlueButtonPalette(browseInput);
    applyBlueButtonPalette(browseOutput);
    applyBlueButtonPalette(startButton);
    applyBlueButtonPalette(licenseBtn);
    applyBlueButtonPalette(noticeBtn);

    QObject::connect(browseInput, &QPushButton::clicked, [&]() {
        QString path = QFileDialog::getOpenFileName(nullptr, "Eingabedatei wählen", "", "CSV-Dateien (*.csv)");
        if (!path.isEmpty()) inputLine->setText(path);
    });

    QObject::connect(browseOutput, &QPushButton::clicked, [&]() {
        QString path = QFileDialog::getSaveFileName(nullptr, "Ausgabedatei wählen", "", "CSV-Dateien (*.csv)");
        if (!path.isEmpty()) outputLine->setText(path);
    });

    QObject::connect(startButton, &QPushButton::clicked, [&]() {
        if (inputLine->text().isEmpty() || outputLine->text().isEmpty()) {
            QMessageBox::warning(nullptr, "Fehlende Angaben", "Bitte Eingabe- und Ausgabedatei angeben.");
            return;
        }
        transform_csv(inputLine->text(), outputLine->text());
    });

    QObject::connect(licenseBtn, &QPushButton::clicked, []() {
        QMessageBox::information(nullptr, "Lizenz – GPLv3", GPL_TEXT);
    });

    QObject::connect(noticeBtn, &QPushButton::clicked, []() {
        QMessageBox::information(nullptr, "Achtung", NOTICE_TEXT);
    });

    // Save original palette to restore later
    QPalette originalPalette = app.palette();

    // Setup initial state without triggering signals
    darkModeToggle->blockSignals(true);
    darkModeToggle->setChecked(true);
    app.setPalette(createDarkPalette());
    applyDarkStyle(&window);
    darkModeToggle->blockSignals(false);

    QObject::connect(darkModeToggle, &QCheckBox::toggled, [&](bool checked) {
        if (checked) {
            app.setPalette(createDarkPalette());
            applyDarkStyle(&window);
        } else {
            app.setPalette(originalPalette);
            applyLightStyle(&window);
        }
    });

    window.show();
    return app.exec();
}
