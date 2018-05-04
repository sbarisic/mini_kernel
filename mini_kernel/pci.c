#include "mini_kernel.h"
#include "pci.h"

// THIS CODE HAS BEEN "STOLEN" FROM THE FOLLOWING GITHUB REPOSITORY
// https://github.com/pdoane/osdev
// WHICH IS LICENSED UNDER ZLIB

#define PCI_CHECK_CLASS_NAME(info, name) (((info.classCode << 8) | info.subclass) == name)

const char* pci_device_name(uint32_t vendorId, uint32_t deviceId) {
	return "Unknown Device";
}

// ------------------------------------------------------------------------------------------------
const char* pci_class_name(uint32_t classCode, uint32_t subclass, uint32_t progIntf) {
	switch ((classCode << 8) | subclass) {
		case PCI_VGA_COMPATIBLE:            return "VGA-Compatible Device";
		case PCI_STORAGE_SCSI:              return "SCSI Storage Controller";
		case PCI_STORAGE_IDE:               return "IDE Interface";
		case PCI_STORAGE_FLOPPY:            return "Floppy Disk Controller";
		case PCI_STORAGE_IPI:               return "IPI Bus Controller";
		case PCI_STORAGE_RAID:              return "RAID Bus Controller";
		case PCI_STORAGE_ATA:               return "ATA Controller";
		case PCI_STORAGE_SATA:              return "SATA Controller";
		case PCI_STORAGE_OTHER:             return "Mass Storage Controller";
		case PCI_NETWORK_ETHERNET:          return "Ethernet Controller";
		case PCI_NETWORK_TOKEN_RING:        return "Token Ring Controller";
		case PCI_NETWORK_FDDI:              return "FDDI Controller";
		case PCI_NETWORK_ATM:               return "ATM Controller";
		case PCI_NETWORK_ISDN:              return "ISDN Controller";
		case PCI_NETWORK_WORLDFIP:          return "WorldFip Controller";
		case PCI_NETWORK_PICGMG:            return "PICMG Controller";
		case PCI_NETWORK_OTHER:             return "Network Controller";
		case PCI_DISPLAY_VGA:               return "VGA-Compatible Controller";
		case PCI_DISPLAY_XGA:               return "XGA-Compatible Controller";
		case PCI_DISPLAY_3D:                return "3D Controller";
		case PCI_DISPLAY_OTHER:             return "Display Controller";
		case PCI_MULTIMEDIA_VIDEO:          return "Multimedia Video Controller";
		case PCI_MULTIMEDIA_AUDIO:          return "Multimedia Audio Controller";
		case PCI_MULTIMEDIA_PHONE:          return "Computer Telephony Device";
		case PCI_MULTIMEDIA_AUDIO_DEVICE:   return "Audio Device";
		case PCI_MULTIMEDIA_OTHER:          return "Multimedia Controller";
		case PCI_MEMORY_RAM:                return "RAM Memory";
		case PCI_MEMORY_FLASH:              return "Flash Memory";
		case PCI_MEMORY_OTHER:              return "Memory Controller";
		case PCI_BRIDGE_HOST:               return "Host Bridge";
		case PCI_BRIDGE_ISA:                return "ISA Bridge";
		case PCI_BRIDGE_EISA:               return "EISA Bridge";
		case PCI_BRIDGE_MCA:                return "MicroChannel Bridge";
		case PCI_BRIDGE_PCI:                return "PCI Bridge";
		case PCI_BRIDGE_PCMCIA:             return "PCMCIA Bridge";
		case PCI_BRIDGE_NUBUS:              return "NuBus Bridge";
		case PCI_BRIDGE_CARDBUS:            return "CardBus Bridge";
		case PCI_BRIDGE_RACEWAY:            return "RACEway Bridge";
		case PCI_BRIDGE_OTHER:              return "Bridge Device";
		case PCI_COMM_SERIAL:               return "Serial Controller";
		case PCI_COMM_PARALLEL:             return "Parallel Controller";
		case PCI_COMM_MULTIPORT:            return "Multiport Serial Controller";
		case PCI_COMM_MODEM:                return "Modem";
		case PCI_COMM_OTHER:                return "Communication Controller";
		case PCI_SYSTEM_PIC:                return "PIC";
		case PCI_SYSTEM_DMA:                return "DMA Controller";
		case PCI_SYSTEM_TIMER:              return "Timer";
		case PCI_SYSTEM_RTC:                return "RTC";
		case PCI_SYSTEM_PCI_HOTPLUG:        return "PCI Hot-Plug Controller";
		case PCI_SYSTEM_SD:                 return "SD Host Controller";
		case PCI_SYSTEM_OTHER:              return "System Peripheral";
		case PCI_INPUT_KEYBOARD:            return "Keyboard Controller";
		case PCI_INPUT_PEN:                 return "Pen Controller";
		case PCI_INPUT_MOUSE:               return "Mouse Controller";
		case PCI_INPUT_SCANNER:             return "Scanner Controller";
		case PCI_INPUT_GAMEPORT:            return "Gameport Controller";
		case PCI_INPUT_OTHER:               return "Input Controller";
		case PCI_DOCKING_GENERIC:           return "Generic Docking Station";
		case PCI_DOCKING_OTHER:             return "Docking Station";
		case PCI_PROCESSOR_386:             return "386";
		case PCI_PROCESSOR_486:             return "486";
		case PCI_PROCESSOR_PENTIUM:         return "Pentium";
		case PCI_PROCESSOR_ALPHA:           return "Alpha";
		case PCI_PROCESSOR_MIPS:            return "MIPS";
		case PCI_PROCESSOR_CO:              return "CO-Processor";
		case PCI_SERIAL_FIREWIRE:           return "FireWire (IEEE 1394)";
		case PCI_SERIAL_SSA:                return "SSA";
		case PCI_SERIAL_USB:
			switch (progIntf)
			{
				case PCI_SERIAL_USB_UHCI:       return "USB (UHCI)";
				case PCI_SERIAL_USB_OHCI:       return "USB (OHCI)";
				case PCI_SERIAL_USB_EHCI:       return "USB2";
				case PCI_SERIAL_USB_XHCI:       return "USB3";
				case PCI_SERIAL_USB_OTHER:      return "USB Controller";
				default:                        return "Unknown USB Class";
			}
			break;
		case PCI_SERIAL_FIBER:              return "Fiber Channel";
		case PCI_SERIAL_SMBUS:              return "SMBus";
		case PCI_WIRELESS_IRDA:             return "iRDA Compatible Controller";
		case PCI_WIRLESSS_IR:               return "Consumer IR Controller";
		case PCI_WIRLESSS_RF:               return "RF Controller";
		case PCI_WIRLESSS_BLUETOOTH:        return "Bluetooth";
		case PCI_WIRLESSS_BROADBAND:        return "Broadband";
		case PCI_WIRLESSS_ETHERNET_A:       return "802.1a Controller";
		case PCI_WIRLESSS_ETHERNET_B:       return "802.1b Controller";
		case PCI_WIRELESS_OTHER:            return "Wireless Controller";
		case PCI_INTELLIGENT_I2O:           return "I2O Controller";
		case PCI_SATELLITE_TV:              return "Satellite TV Controller";
		case PCI_SATELLITE_AUDIO:           return "Satellite Audio Controller";
		case PCI_SATELLITE_VOICE:           return "Satellite Voice Controller";
		case PCI_SATELLITE_DATA:            return "Satellite Data Controller";
		case PCI_CRYPT_NETWORK:             return "Network and Computing Encryption Device";
		case PCI_CRYPT_ENTERTAINMENT:       return "Entertainment Encryption Device";
		case PCI_CRYPT_OTHER:               return "Encryption Device";
		case PCI_SP_DPIO:                   return "DPIO Modules";
		case PCI_SP_OTHER:                  return "Signal Processing Controller";
	}

	return "Unknown PCI Class";
}

uint8_t pci_read8(uint32_t id, uint32_t reg) {
	uint32_t addr = 0x80000000 | id | (reg & 0xfc);
	__outdword(PCI_CONFIG_ADDR, addr);
	return __inbyte(PCI_CONFIG_DATA + (reg & 0x03));
}

uint16_t pci_read16(uint32_t id, uint32_t reg) {
	uint32_t addr = 0x80000000 | id | (reg & 0xfc);
	__outdword(PCI_CONFIG_ADDR, addr);
	return __inword(PCI_CONFIG_DATA + (reg & 0x02));
}

uint32_t pci_read32(uint32_t id, uint32_t reg) {
	uint32_t addr = 0x80000000 | id | (reg & 0xfc);
	__outdword(PCI_CONFIG_ADDR, addr);
	return __indword(PCI_CONFIG_DATA);
}

void pci_write8(uint32_t id, uint32_t reg, uint8_t data) {
	uint32_t address = 0x80000000 | id | (reg & 0xfc);
	__outdword(PCI_CONFIG_ADDR, address);
	__outbyte(PCI_CONFIG_DATA + (reg & 0x03), data);
}

void pci_write16(uint32_t id, uint32_t reg, uint16_t data) {
	uint32_t address = 0x80000000 | id | (reg & 0xfc);
	__outdword(PCI_CONFIG_ADDR, address);
	__outword(PCI_CONFIG_DATA + (reg & 0x02), data);
}

// ------------------------------------------------------------------------------------------------
void pci_write32(uint32_t id, uint32_t reg, uint32_t data) {
	uint32_t address = 0x80000000 | id | (reg & 0xfc);
	__outdword(PCI_CONFIG_ADDR, address);
	__outdword(PCI_CONFIG_DATA, data);
}

// ------------------------------------------------------------------------------------------------
static void pci_readbar(uint32_t id, uint32_t index, uint32_t *address, uint32_t *mask) {
	uint32_t reg = PCI_CONFIG_BAR0 + index * sizeof(uint32_t);

	// Get address
	*address = pci_read32(id, reg);

	// Find out size of the bar
	pci_write32(id, reg, 0xffffffff);
	*mask = pci_read32(id, reg);

	// Restore adddress
	pci_write32(id, reg, *address);
}

// ------------------------------------------------------------------------------------------------
void pci_getbar(PciBar *bar, uint32_t id, uint32_t index) {
	// Read pci bar register
	uint32_t addressLow;
	uint32_t maskLow;
	pci_readbar(id, index, &addressLow, &maskLow);

	if (addressLow & PCI_BAR_64) {
		// 64-bit mmio
		uint32_t addressHigh;
		uint32_t maskHigh;
		pci_readbar(id, index + 1, &addressHigh, &maskHigh);

		bar->u.address = (void *)(((uintptr_t)addressHigh << 32) | (addressLow & ~0xf));
		bar->size = ~(((uint64_t)maskHigh << 32) | (maskLow & ~0xf)) + 1;
		bar->flags = addressLow & 0xf;
	} else if (addressLow & PCI_BAR_IO) {
		// i/o register
		bar->u.port = (uint16_t)(addressLow & ~0x3);
		bar->size = (uint16_t)(~(maskLow & ~0x3) + 1);
		bar->flags = addressLow & 0x3;
	} else {
		// 32-bit mmio
		bar->u.address = (void *)(uintptr_t)(addressLow & ~0xf);
		bar->size = ~(maskLow & ~0xf) + 1;
		bar->flags = addressLow & 0xf;
	}
}

static int pci_print_counter;

static void pci_visit(uint32_t bus, uint32_t dev, uint32_t func, int print_only) {
	uint32_t id = PCI_MAKE_ID(bus, dev, func);
	char buffer[512] = { 0 };

	PciDeviceInfo info;
	info.vendorId = pci_read16(id, PCI_CONFIG_VENDOR_ID);
	if (info.vendorId == 0xffff)
		return;

	info.deviceId = pci_read16(id, PCI_CONFIG_DEVICE_ID);
	info.progIntf = pci_read8(id, PCI_CONFIG_PROG_INTF);
	info.subclass = pci_read8(id, PCI_CONFIG_SUBCLASS);
	info.classCode = pci_read8(id, PCI_CONFIG_CLASS_CODE);

	if (print_only) {
		stbsp_sprintf(buffer, "%02d - %02x:%02x:%d %04X:%04X  %s\n", pci_print_counter++, bus, dev, func, info.vendorId, info.deviceId,
					  pci_class_name(info.classCode, info.subclass, info.progIntf));
		write(buffer);
	} else {
		if (PCI_CHECK_CLASS_NAME(info, PCI_COMM_SERIAL)) {
			//pci_write32(id,  PCI_CONFIG_COMMAND,)

			PciBar bar0;
			pci_getbar(&bar0, id, 0);
			init_com(bar0.u.port);

			/*for (uint32_t i = 0; i < 10; i++) {
				PciBar bar;
				pci_getbar(&bar, id, i * 2); // IO base

				stbsp_sprintf(buffer, "BAR%d %x/%x %ld %d\n", i * 2, bar.u.address, bar.u.port, bar.size, bar.flags);
				write(buffer);
			}*/
		}
	}
}

void pci_enumerate_all(int print_only) {
	pci_print_counter = 0;

	for (uint32_t bus = 0; bus < 256; ++bus) {
		for (uint32_t dev = 0; dev < 32; ++dev) {
			uint32_t baseId = PCI_MAKE_ID(bus, dev, 0);
			uint8_t headerType = pci_read8(baseId, PCI_CONFIG_HEADER_TYPE);
			uint32_t funcCount = headerType & PCI_TYPE_MULTIFUNC ? 8 : 1;

			for (uint32_t func = 0; func < funcCount; ++func)
				pci_visit(bus, dev, func, print_only);
		}
	}
}

void cmd_lspci(int argc, const char** argv, const char* argi) {
	pci_enumerate_all(1);
}

void pci_init() {
	register_console_command("lspci", &cmd_lspci);
	pci_enumerate_all(0);
}