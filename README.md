# 🖨️ SaneScanner-PapplRetrofit

SaneScanner-PapplRetrofit is a dynamic project that lets users seamlessly connect with scanners. Whether you wish to interact directly or desire server-mode operation, Sane Scanner has got you covered! It harnesses the power of the **SANE** (Scanner Access Now Easy) library and **Microhttpd** for scan services.

> 🚧 **Work in Progress**: This project is actively being developed. Some parts may not function as expected. Stay tuned for updates!

## 🛠 Prerequisites

Before diving in, make sure these are installed:
- **sane-backends**: Essential for SANE library magic.
- **libmicrohttpd**: Powers the Microhttpd HTTP server operations.

## ⚙️ Building the Project

🚀 Get started with these easy steps:

1. **Clone & Enter the Directory**:
```bash
git clone [FORKED_REPOSITORY_URL]
cd Sane_Scanner
```

2. **Run the Magic Script**:
```bash
chmod +x autogen.sh
./autogen.sh
```

3. **Configure**:
```bash
./configure
```

4. **Build Away**:
```bash
make
```

## 🏃 Running the Application

Post-build, embark on the scanning journey:

```bash
./sane_scanner
```

A friendly prompt will ask if you prefer a direct scanner connection or server-mode. Choose and voyage!

