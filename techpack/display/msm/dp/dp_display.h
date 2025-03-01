/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.
 * Copyright (c) 2017-2021, The Linux Foundation. All rights reserved.
 */

#ifndef _DP_DISPLAY_H_
#define _DP_DISPLAY_H_

#include <linux/list.h>
#include <drm/drmP.h>
#include <drm/msm_drm.h>

#include "dp_panel.h"
#if defined(CONFIG_LGE_DUAL_SCREEN)
#include <linux/extcon.h>
#define EXT_DD_MAX_COUNT 3
#endif
#ifdef CONFIG_LGE_DISPLAY_COMMON
#include "../lge/dp/lge_dp_def.h"
#endif

#define DP_MST_SIM_MAX_PORTS	8

enum dp_drv_state {
	PM_DEFAULT,
	PM_SUSPEND,
};

struct dp_mst_hpd_info {
	bool mst_protocol;
	bool mst_hpd_sim;
	u32 mst_port_cnt;
	u8 *edid;
	bool mst_sim_add_con;
	bool mst_sim_remove_con;
	int mst_sim_remove_con_id;
};

struct dp_mst_drm_cbs {
	void (*hpd)(void *display, bool hpd_status,
			struct dp_mst_hpd_info *info);
	void (*hpd_irq)(void *display, struct dp_mst_hpd_info *info);
	void (*set_drv_state)(void *dp_display,
			enum dp_drv_state mst_state);
};

struct dp_mst_drm_install_info {
	void *dp_mst_prv_info;
	const struct dp_mst_drm_cbs *cbs;
};

struct dp_mst_caps {
	bool has_mst;
	u32 max_streams_supported;
	u32 max_dpcd_transaction_bytes;
	struct drm_dp_aux *drm_aux;
};

struct dp_mst_connector {
	bool debug_en;
	int con_id;
	int hdisplay;
	int vdisplay;
	int vrefresh;
	int aspect_ratio;
	struct drm_connector *conn;
	struct mutex lock;
	struct list_head list;
	enum drm_connector_status state;
};

struct dp_display {
	struct drm_device *drm_dev;
	struct dp_bridge *bridge;
	struct drm_connector *base_connector;
	void *base_dp_panel;
	bool is_sst_connected;
	bool is_mst_supported;
	u32 max_pclk_khz;
	u32 max_hdisplay;
	u32 max_vdisplay;
	u32 no_mst_encoder;
	void *dp_mst_prv_info;
	bool is_primary;

	int (*enable)(struct dp_display *dp_display, void *panel);
	int (*post_enable)(struct dp_display *dp_display, void *panel);

	int (*pre_disable)(struct dp_display *dp_display, void *panel);
	int (*disable)(struct dp_display *dp_display, void *panel);

	int (*set_mode)(struct dp_display *dp_display, void *panel,
			struct dp_display_mode *mode);
	enum drm_mode_status (*validate_mode)(struct dp_display *dp_display,
			void *panel, struct drm_display_mode *mode,
			const struct msm_resource_caps_info *avail_res);
	int (*get_modes)(struct dp_display *dp_display, void *panel,
		struct dp_display_mode *dp_mode);
	int (*prepare)(struct dp_display *dp_display, void *panel);
	int (*unprepare)(struct dp_display *dp_display, void *panel);
	int (*request_irq)(struct dp_display *dp_display);
	struct dp_debug *(*get_debug)(struct dp_display *dp_display);
	void (*post_open)(struct dp_display *dp_display);
	int (*config_hdr)(struct dp_display *dp_display, void *panel,
				struct drm_msm_ext_hdr_metadata *hdr_meta,
				bool dhdr_update);
	int (*set_colorspace)(struct dp_display *dp_display, void *panel,
				u32 colorspace);
	int (*post_init)(struct dp_display *dp_display);
	int (*mst_install)(struct dp_display *dp_display,
			struct dp_mst_drm_install_info *mst_install_info);
	int (*mst_uninstall)(struct dp_display *dp_display);
	int (*mst_connector_install)(struct dp_display *dp_display,
			struct drm_connector *connector);
	int (*mst_connector_uninstall)(struct dp_display *dp_display,
			struct drm_connector *connector);
	int (*mst_connector_update_edid)(struct dp_display *dp_display,
			struct drm_connector *connector,
			struct edid *edid);
	int (*mst_connector_update_link_info)(struct dp_display *dp_display,
			struct drm_connector *connector);
	int (*mst_get_connector_info)(struct dp_display *dp_display,
			struct drm_connector *connector,
			struct dp_mst_connector *mst_conn);
	int (*mst_get_fixed_topology_port)(struct dp_display *dp_display,
			u32 strm_id, u32 *port_num);
	int (*get_mst_caps)(struct dp_display *dp_display,
			struct dp_mst_caps *mst_caps);
	int (*set_stream_info)(struct dp_display *dp_display, void *panel,
			u32 strm_id, u32 start_slot, u32 num_slots, u32 pbn,
			int vcpi);
	void (*convert_to_dp_mode)(struct dp_display *dp_display, void *panel,
			const struct drm_display_mode *drm_mode,
			struct dp_display_mode *dp_mode);
	int (*update_pps)(struct dp_display *dp_display,
			struct drm_connector *connector, char *pps_cmd);
	void (*wakeup_phy_layer)(struct dp_display *dp_display,
			bool wakeup);
	int (*get_display_type)(struct dp_display *dp_display,
			const char **display_type);

#ifdef CONFIG_LGE_DISPLAY_COMMON
	struct lge_dp_display lge_dp;
#endif
};

#ifdef CONFIG_LGE_DISPLAY_COMMON
int dp_display_external_block(struct lge_dp_display *lge_dp, int block);
int dp_display_send_id_event(struct lge_dp_display *lge_dp);
#endif
#if defined(CONFIG_LGE_DUAL_SCREEN)
int is_dd_connected(void);
#endif

#ifdef CONFIG_DRM_MSM_DP
int dp_display_get_num_of_displays(void);
int dp_display_get_displays(void **displays, int count);
int dp_display_get_num_of_streams(void);
#else
static inline int dp_display_get_num_of_displays(void)
{
	return 0;
}
static inline int dp_display_get_displays(void **displays, int count)
{
	return 0;
}
static inline int dp_display_get_num_of_streams(void)
{
	return 0;
}
static inline int dp_connector_update_pps(struct drm_connector *connector,
		char *pps_cmd, void *display)
{
	return 0;
}
#endif
#endif /* _DP_DISPLAY_H_ */
