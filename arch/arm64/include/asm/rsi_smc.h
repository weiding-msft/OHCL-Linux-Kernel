/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2023 ARM Ltd.
 */

#ifndef __ASM_RSI_SMC_H_
#define __ASM_RSI_SMC_H_

#include <linux/arm-smccc.h>

/*
 * This file describes the Realm Services Interface (RSI) Application Binary
 * Interface (ABI) for SMC calls made from within the Realm to the RMM and
 * serviced by the RMM.
 */

/*
 * The major version number of the RSI implementation.  This is increased when
 * the binary format or semantics of the SMC calls change.
 */
#define RSI_ABI_VERSION_MAJOR		UL(1)

/*
 * The minor version number of the RSI implementation.  This is increased when
 * a bug is fixed, or a feature is added without breaking binary compatibility.
 */
#define RSI_ABI_VERSION_MINOR		UL(0)

#define RSI_ABI_VERSION			((RSI_ABI_VERSION_MAJOR << 16) | \
					 RSI_ABI_VERSION_MINOR)

#define RSI_ABI_VERSION_GET_MAJOR(_version) ((_version) >> 16)
#define RSI_ABI_VERSION_GET_MINOR(_version) ((_version) & 0xFFFF)

#define RSI_SUCCESS		UL(0)
#define RSI_ERROR_INPUT		UL(1)
#define RSI_ERROR_STATE		UL(2)
#define RSI_INCOMPLETE		UL(3)
#define RSI_ERROR_UNKNOWN	UL(4)

#define SMC_RSI_FID(n)		ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,      \
						   ARM_SMCCC_SMC_64,         \
						   ARM_SMCCC_OWNER_STANDARD, \
						   n)

/* Number of general purpose registers per Plane */
#define RSI_PLANE_NR_GPRS		31

/* Maximum number of Interrupt Controller List Registers */
#define RSI_PLANE_GIC_NUM_LRS		16

/*
 * Returns RSI version.
 *
 * arg1 == Requested interface revision
 * ret0 == Status / error
 * ret1 == Lower implemented interface revision
 * ret2 == Higher implemented interface revision
 */
#define SMC_RSI_ABI_VERSION	SMC_RSI_FID(0x190)

/*
 * Read feature register.
 *
 * arg1 == Feature register index
 * ret0 == Status / error
 * ret1 == Feature register value
 */
#define SMC_RSI_FEATURES			SMC_RSI_FID(0x191)

/*
 * Read measurement for the current Realm.
 *
 * arg1 == Index, which measurements slot to read
 * ret0 == Status / error
 * ret1 == Measurement value, bytes:  0 -  7
 * ret2 == Measurement value, bytes:  8 - 15
 * ret3 == Measurement value, bytes: 16 - 23
 * ret4 == Measurement value, bytes: 24 - 31
 * ret5 == Measurement value, bytes: 32 - 39
 * ret6 == Measurement value, bytes: 40 - 47
 * ret7 == Measurement value, bytes: 48 - 55
 * ret8 == Measurement value, bytes: 56 - 63
 */
#define SMC_RSI_MEASUREMENT_READ		SMC_RSI_FID(0x192)

/*
 * Extend Realm Extensible Measurement (REM) value.
 *
 * arg1  == Index, which measurements slot to extend
 * arg2  == Size of realm measurement in bytes, max 64 bytes
 * arg3  == Measurement value, bytes:  0 -  7
 * arg4  == Measurement value, bytes:  8 - 15
 * arg5  == Measurement value, bytes: 16 - 23
 * arg6  == Measurement value, bytes: 24 - 31
 * arg7  == Measurement value, bytes: 32 - 39
 * arg8  == Measurement value, bytes: 40 - 47
 * arg9  == Measurement value, bytes: 48 - 55
 * arg10 == Measurement value, bytes: 56 - 63
 * ret0  == Status / error
 */
#define SMC_RSI_MEASUREMENT_EXTEND		SMC_RSI_FID(0x193)

/*
 * Initialize the operation to retrieve an attestation token.
 *
 * arg1 == Challenge value, bytes:  0 -  7
 * arg2 == Challenge value, bytes:  8 - 15
 * arg3 == Challenge value, bytes: 16 - 23
 * arg4 == Challenge value, bytes: 24 - 31
 * arg5 == Challenge value, bytes: 32 - 39
 * arg6 == Challenge value, bytes: 40 - 47
 * arg7 == Challenge value, bytes: 48 - 55
 * arg8 == Challenge value, bytes: 56 - 63
 * ret0 == Status / error
 * ret1 == Upper bound of token size in bytes
 */
#define SMC_RSI_ATTESTATION_TOKEN_INIT		SMC_RSI_FID(0x194)

/*
 * Continue the operation to retrieve an attestation token.
 *
 * arg1 == The IPA of token buffer
 * arg2 == Offset within the granule of the token buffer
 * arg3 == Size of the granule buffer
 * ret0 == Status / error
 * ret1 == Length of token bytes copied to the granule buffer
 */
#define SMC_RSI_ATTESTATION_TOKEN_CONTINUE	SMC_RSI_FID(0x195)
/*
 * arg0 == Plane index
 * arg1 == Permission Index
 * arg2 == Permission
 */
#define SMC_RSI_MEM_SET_PERM_VALUE	SMC_RSI_FID(0x1A2)

/*
 * arg1 == Base address of IPA region
 * arg2 == Size of IPA region in bytes
 * arg3 == Permission Index
 * arg4 == Cookie
 * ret1 == New cookie value
 */
#define SMC_RSI_MEM_SET_PERM_INDEX	SMC_RSI_FID(0x1A1)

/*
 * arg0 == Plane index
 * arg1 == struct rsi_plane_run addr
 */
#define SMC_RSI_PLANE_ENTER			SMC_RSI_FID(0x1A3)

/*
 * arg0 == Architecturally-defined sysreg address
 * arg1 == Value
 */
#define SMC_RSI_PLANE_SYSREG_WRITE		SMC_RSI_FID(0x1AF)

#ifndef __ASSEMBLY__

struct realm_config {
	union {
		struct {
			unsigned long ipa_bits; /* Width of IPA in bits */
			unsigned long hash_algo; /* Hash algorithm */
		};
		u8 pad[0x200];
	};
	union {
		u8 rpv[64]; /* Realm Personalization Value */
		u8 pad2[0xe00];
	};
	/*
	 * The RMM requires the configuration structure to be aligned to a 4k
	 * boundary, ensure this happens by aligning this structure.
	 */
} __aligned(0x1000);


/*
 * EL1 system registers per Plane
 */
struct rsi_plane_el1_sysregs {
	unsigned long sp_el0;			/*   0x0 */
	unsigned long sp_el1;			/*   0x8 */
	unsigned long elr_el1;			/*  0x10 */
	unsigned long spsr_el1;			/*  0x18 */
	unsigned long pmcr_el0;			/*  0x20 */
	unsigned long pmuserenr_el0;		/*  0x28 */
	unsigned long tpidrro_el0;		/*  0x30 */
	unsigned long tpidr_el0;		/*  0x38 */
	unsigned long csselr_el1;		/*  0x40 */
	unsigned long sctlr_el1;		/*  0x48 */
	unsigned long actlr_el1;		/*  0x50 */
	unsigned long cpacr_el1;		/*  0x58 */
	unsigned long zcr_el1;			/*  0x60 */
	unsigned long ttbr0_el1;		/*  0x68 */
	unsigned long ttbr1_el1;		/*  0x70 */
	unsigned long tcr_el1;			/*  0x78 */
	unsigned long esr_el1;			/*  0x80 */
	unsigned long afsr0_el1;		/*  0x88 */
	unsigned long afsr1_el1;		/*  0x90 */
	unsigned long far_el1;			/*  0x98 */
	unsigned long mair_el1;			/*  0xA0 */
	unsigned long vbar_el1;			/*  0xA8 */
	unsigned long contextidr_el1;		/*  0xB0 */
	unsigned long tpidr_el1;		/*  0xB8 */
	unsigned long amair_el1;		/*  0xC0 */
	unsigned long cntkctl_el1;		/*  0xC8 */
	unsigned long par_el1;			/*  0xD0 */
	unsigned long mdscr_el1;		/*  0xD8 */
	unsigned long mdccint_el1;		/*  0xE0 */
	unsigned long disr_el1;			/*  0xE8 */
	unsigned long mpam0_el1;		/*  0xF0 */

	/* Timer Registers */
	unsigned long cntp_ctl_el0;		/*  0xF8 */
	unsigned long cntp_cval_el0;		/* 0x100 */
	unsigned long cntv_ctl_el0;		/* 0x108 */
	unsigned long cntv_cval_el0;		/* 0x110 */
};

/*
 * Data passed from P0 to the RMM on entry to Pn
 */
struct rsi_plane_entry {
	union {
		struct {
			unsigned long flags;				/* 0x000 */
			unsigned long pc;				/* 0x008 */
		};
		unsigned char __reserved0[0x100];
	};/* 0x0 - 0x100 */
	union {
		struct {
			unsigned long gprs[RSI_PLANE_NR_GPRS];		/* 0x100 */
		};
		unsigned char __reserved1[0x100];
	};/* 0x100 - 0x200 */
	union {
		struct {
			unsigned long gicv3_hcr;			/* 0x200 */
			unsigned long gicv3_lrs[RSI_PLANE_GIC_NUM_LRS];	/* 0x208 */
		};
		unsigned char __reserved3[0x100];
	};/* 0x200 - 0x300 */
};

/*
 * Data passed from the RMM to P0 on exit from Pn
 */
struct rsi_plane_exit {
	union {
		struct {
			unsigned long exit_reason;			/* 0x000 */
		};
		unsigned char __reserved0[0x100];
	};/* 0x0 - 0x100 */
	union {
		struct {
			unsigned long elr_el2;				/* 0x100 */
			unsigned long esr_el2;				/* 0x108 */
			unsigned long far_el2;				/* 0x110 */
			unsigned long hpfar_el2;			/* 0x118 */
		};
		unsigned char __reserved1[0x100];
	};/* 0x100 - 0x200 */
	union {
		struct {
			unsigned long gprs[RSI_PLANE_NR_GPRS];		/* 0x200 */
		};
		unsigned char __reserved2[0x100];
	};/* 0x200 - 0x300 */
	union {
		struct {
			unsigned long gicv3_hcr;			/* 0x300 */
			unsigned long gicv3_lrs[RSI_PLANE_GIC_NUM_LRS];	/* 0x308 */
			unsigned long gicv3_misr;			/* 0x388 */
			unsigned long gicv3_vmcr;			/* 0x390 */
			unsigned long cntp_ctl_el0;				/* 0x398 */
			unsigned long cntp_cval_el0;			/* 0x3a0 */
			unsigned long cntv_ctl_el0;				/* 0x3a8 */
			unsigned long cntv_cval_el0;			/* 0x3b0 */
		};
		unsigned char __reserved4[0x100];
	};/* 0x300 - 0x400 */
};

/*
 * Data shared between P0 and the RMM during entry to and exit from Pn
 */
struct rsi_plane_run {
	union {
		struct {
			struct rsi_plane_entry entry;			/* 0x000 */
		};
		unsigned char __reserved0[0x800];
	};/* 0x000 - 0x800 */
	union {
		struct {
			struct rsi_plane_exit exit; 			/* 0x800 */
		};
		unsigned char __reserved1[0x800];
	};/* 0x800 - 0x1000*/
} __aligned(0x1000);

#endif /* __ASSEMBLY__ */

/*
 * Read configuration for the current Realm.
 *
 * arg1 == struct realm_config addr
 * ret0 == Status / error
 */
#define SMC_RSI_REALM_CONFIG			SMC_RSI_FID(0x196)

/*
 * Request RIPAS of a target IPA range to be changed to a specified value.
 *
 * arg1 == Base IPA address of target region
 * arg2 == Top of the region
 * arg3 == RIPAS value
 * arg4 == flags
 * ret0 == Status / error
 * ret1 == Top of modified IPA range
 * ret2 == Whether the Host accepted or rejected the request
 */
#define SMC_RSI_IPA_STATE_SET			SMC_RSI_FID(0x197)

#define RSI_NO_CHANGE_DESTROYED			UL(0)
#define RSI_CHANGE_DESTROYED			UL(1)

#define RSI_ACCEPT				UL(0)
#define RSI_REJECT				UL(1)

/*
 * Get RIPAS of a target IPA range.
 *
 * arg1 == Base IPA of target region
 * arg2 == End of target IPA region
 * ret0 == Status / error
 * ret1 == Top of IPA region which has the reported RIPAS value
 * ret2 == RIPAS value
 */
#define SMC_RSI_IPA_STATE_GET			SMC_RSI_FID(0x198)

/*
 * Make a Host call.
 *
 * arg1 == IPA of host call structure
 * ret0 == Status / error
 */
#define SMC_RSI_HOST_CALL			SMC_RSI_FID(0x199)

#endif /* __ASM_RSI_SMC_H_ */
