xgp_browser_icons = files(
  'Browser_epiphany.png', 'Browser_firefox.png', 'Browser_galeon.png', 'Browser_mozilla.png',
  'Browser_opera.png', 'Browser_netscape.png', 'Browser_konqueror.png',
  'Browser_GTKMOZEMBED.png', 'Browser_GTKHTML.png', 'Browser_seamonkey.png',
  'Browser_dillo.png', 'Browser_WEBKIT.png',
)
install_data(xgp_browser_icons, install_dir: pkgdatadir_xgp)

# Without giomm the file icons are handled by XGP itself
if not giomm_dep.found()
  xgp_file_icons = files(
    'Default.png', 'Directory.png', 'Executable.png', 'Icon_3ds.png', 'Icon_ac3.png',
    'Icon_afm.png', 'Icon_ag.png', 'Icon_aiff.png', 'Icon_arj.png', 'Icon_asf.png',
    'Icon_au.png', 'Icon_AUTHORS.png', 'Icon_avi.png', 'Icon_bak.png', 'Icon_bdf.png',
    'Icon_blend.png', 'Icon_bmp.png', 'Icon_bmx.png', 'Icon_c.png', 'Icon_cc-source.png',
    'Icon_COPYING.png', 'Icon_core.png', 'Icon_cpp.png', 'Icon_CREDITS.png', 'Icon_deb.png',
    'Icon_dia.png', 'Icon_doc.png', 'Icon_eps.png', 'Icon_gif.png', 'Icon_gnumeric.png',
    'Icon_h.png', 'Icon_htm.png', 'Icon_html.png', 'Icon_ief.png', 'Icon_INSTALL.png',
    'Icon_jar.png', 'Icon_java.png', 'Icon_jpeg.png', 'Icon_jpg.png', 'Icon_kil.png',
    'Icon_kpr.png', 'Icon_ksp.png', 'Icon_kwd.png', 'Icon_lyx.png', 'Icon_Makefile.png',
    'Icon_mid.png', 'Icon_mod.png', 'Icon_mov.png', 'Icon_mp3.png', 'Icon_mpeg.png',
    'Icon_mpg.png', 'Icon_mrp.png', 'Icon_o.png', 'Icon_odg.png', 'Icon_odp.png',
    'Icon_ods.png', 'Icon_odt.png', 'Icon_ogg.png', 'Icon_pbm.png', 'Icon_pcf.png',
    'Icon_pdf.png', 'Icon_pgm.png', 'Icon_pgp.png', 'Icon_php.png', 'Icon_png.png',
    'Icon_pnm.png', 'Icon_ppm.png', 'Icon_ppt.png', 'Icon_ps.png', 'Icon_psd.png',
    'Icon_psf.png', 'Icon_qif.png', 'Icon_ra.png', 'Icon_ras.png', 'Icon_README.png',
    'Icon_rej.png', 'Icon_rgb.png', 'Icon_rm.png', 'Icon_rpm.png', 'Icon_rtf.png',
    'Icon_s3m.png', 'Icon_scm.png', 'Icon_sdc.png', 'Icon_sdd.png', 'Icon_sdw.png',
    'Icon_smi.png', 'Icon_so.png', 'Icon_sql.png', 'Icon_stm.png', 'Icon_svg.png',
    'Icon_sxc.png', 'Icon_sxd.png', 'Icon_sxi.png', 'Icon_sxm.png', 'Icon_sxw.png',
    'Icon_tar.png', 'Icon_tex.png', 'Icon_tga.png', 'Icon_tif.png', 'Icon_tiff.png',
    'Icon_ttf.png', 'Icon_txt.png', 'Icon_ulaw.png', 'Icon_url.png', 'Icon_voc.png',
    'Icon_wav.png', 'Icon_wks.png', 'Icon_x-xfig.png', 'Icon_xbm.png', 'Icon_xcf.png',
    'Icon_xi.png', 'Icon_xls.png', 'Icon_xm.png', 'Icon_xpm.png', 'Icon_xwd.png', 'Icon_Z.png',
    'Icon_z.png'
  )
  install_data(xgp_file_icons, install_dir: pkgdatadir_xgp)
endif
