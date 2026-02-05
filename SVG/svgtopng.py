import sys
import os
from PyQt6.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QPushButton,
    QFileDialog, QLabel, QMessageBox, QProgressBar
)
from PyQt6.QtCore import Qt
import cairosvg


class SVGToPNGConverter(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("SVG → PNG Converter")
        self.setGeometry(300, 300, 400, 200)
        self.svg_files = []
        self.output_dir = ""

        layout = QVBoxLayout()

        # Buttons and labels
        self.label_files = QLabel("SVG files: None selected")
        self.btn_select_files = QPushButton("Select SVG Files")
        self.btn_select_files.clicked.connect(self.select_files)

        self.label_out = QLabel("Output directory: Not selected")
        self.btn_select_out = QPushButton("Select Output Directory")
        self.btn_select_out.clicked.connect(self.select_output_dir)

        self.btn_convert = QPushButton("Convert to PNG")
        self.btn_convert.clicked.connect(self.convert_files)
        self.btn_convert.setEnabled(False)

        self.progress = QProgressBar()
        self.progress.setRange(0, 100)
        self.progress.setValue(0)

        # Add to layout
        layout.addWidget(self.label_files)
        layout.addWidget(self.btn_select_files)
        layout.addWidget(self.label_out)
        layout.addWidget(self.btn_select_out)
        layout.addWidget(self.btn_convert)
        layout.addWidget(self.progress)

        self.setLayout(layout)

    def select_files(self):
        files, _ = QFileDialog.getOpenFileNames(
            self, "Select SVG Files", "", "SVG Files (*.svg)"
        )
        if files:
            self.svg_files = files
            self.label_files.setText(f"SVG files: {len(files)} selected")
            self.check_ready()

    def select_output_dir(self):
        dir_path = QFileDialog.getExistingDirectory(self, "Select Output Directory")
        if dir_path:
            self.output_dir = dir_path
            self.label_out.setText(f"Output directory: {dir_path}")
            self.check_ready()

    def check_ready(self):
        self.btn_convert.setEnabled(bool(self.svg_files and self.output_dir))

    def convert_files(self):
        if not self.svg_files or not self.output_dir:
            return

        total = len(self.svg_files)
        errors = []

        for i, svg_path in enumerate(self.svg_files):
            try:
                filename = os.path.basename(svg_path)
                name_no_ext = os.path.splitext(filename)[0]
                png_path = os.path.join(self.output_dir, f"{name_no_ext}.png")
                cairosvg.svg2png(url=svg_path, write_to=png_path)
            except Exception as e:
                errors.append(f"{filename}: {str(e)}")
            finally:
                # Update progress
                self.progress.setValue(int((i + 1) / total * 100))
                QApplication.processEvents()

        # Done
        if errors:
            msg = "Conversion completed with errors:\n" + "\n".join(errors)
            QMessageBox.warning(self, "Finished — with errors", msg)
        else:
            QMessageBox.information(self, "Success", f"All {total} files converted successfully!")

        self.progress.setValue(0)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = SVGToPNGConverter()
    window.show()
    sys.exit(app.exec())