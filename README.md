# cnetstat

cnetstat is a Linux network utilization program written in pure C.
Statistics are collected from the kernel and displayed in a human-readable format.
The collected data persists after reboot.

## Installation

```bash
git clone https://github.com/skyblueborb/cnetstat && cd cnetstat
sudo make install
```
## Usage

```bash
USAGE:
  cnetstat [--help, --gb ...] <network_device>

OPTIONS:
  -h, --help    display this help message
  -b, --kb      prints raw bytes in addition to conversion
  -k, --kb      converts the output to kilobytes
  -m, --mb      converts the output to megabytes
  -g, --gb      converts the output to gigabytes
  -t, --tb      converts the output to terabytes
```

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License
[MIT](https://choosealicense.com/licenses/mit/)
