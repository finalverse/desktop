#!/usr/bin/env python3
# Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
import hashlib
import os
from pathlib import Path
import plistlib
import tempfile
import unittest
from unittest.mock import patch
import xml.etree.ElementTree as ET
from harbor_benchmark import encode
from launch_harbor_benchmark import preflight, GRID_KEY, GRID_URI, REGION_ID

class LaunchSafety(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(); self.root = Path(self.temp.name)
        self.app = self.root/'Fixture.app'; (self.app/'Contents/MacOS').mkdir(parents=True)
        (self.app/'Contents/Info.plist').write_bytes(plistlib.dumps({'CFBundleIdentifier':'com.finalverse.viewer.experience','CFBundleExecutable':'Finalverse Test'}))
        (self.app/'Contents/MacOS/Finalverse Test').write_bytes(b'fixture executable')
        self.digest = hashlib.sha256(b'fixture executable').hexdigest()
        self.profile = self.root/'profile'; self.grid = self.profile/'Library/Application Support/Finalverse/user_settings/grids.xml'; self.grid.parent.mkdir(parents=True)
        self.write(self.grid, {GRID_KEY:{'login_uri':[GRID_URI]}})
        self.session = self.root/'session.xml'; self.spec = self.root/'spec.xml'
        self.settings = {k:{'Value':False} for k in ['AutoLogin','RememberUser','RememberPassword']}
        self.settings['UserLoginInfo'] = {'Value':['Disposable','Fixture','synthetic-test-secret']}
        self.fixture = {'schema':1,'grid_login_uri':GRID_URI,'region_id':REGION_ID,'output_dir':str(self.root/'new-evidence'),'views':[{'name':'CAM_Pavilion'}]}
        self.write(self.session,self.settings); self.write(self.spec,self.fixture)
        self.ps = patch('launch_harbor_benchmark.subprocess.check_output',return_value=''); self.ps.start()
        self.platform = patch('launch_harbor_benchmark.sys.platform','darwin'); self.platform.start()
    def tearDown(self):
        self.platform.stop(); self.ps.stop(); self.temp.cleanup()
    def write(self,path,data):
        root=ET.Element('llsd'); root.append(encode(data)); path.write_bytes(ET.tostring(root)); path.chmod(0o600)
    def run_preflight(self): return preflight(self.app,self.profile,self.session,self.spec,self.digest)
    def test_explicit_grid_profile_and_no_secret_in_argv(self):
        args=self.run_preflight(); self.assertIn(GRID_KEY,args); self.assertIn('CFFIXED_USER_HOME='+str(self.profile.resolve()),args)
        self.assertNotIn('synthetic-test-secret',' '.join(args)); self.assertNotIn('Agni',' '.join(args))
    def test_reject_public_grid_mapping_before_launch(self):
        self.write(self.grid,{GRID_KEY:{'login_uri':['https://login.agni.lindenlab.com/']}})
        with self.assertRaises(ValueError): self.run_preflight()
    def test_reject_session_redirect_and_remember_override(self):
        for key in ['CurrentGrid','CmdLineLoginURI','AutoLogin','RememberPassword']:
            settings=dict(self.settings); settings[key]={'Value':True}; self.write(self.session,settings)
            with self.assertRaises(ValueError): self.run_preflight()
    def test_reject_wrong_executable_and_nonprivate_session(self):
        self.digest='0'*64
        with self.assertRaises(ValueError): self.run_preflight()
        self.digest=hashlib.sha256(b'fixture executable').hexdigest(); self.session.chmod(0o644)
        with self.assertRaises(ValueError): self.run_preflight()
    def test_reject_active_viewer_and_existing_evidence(self):
        with patch('launch_harbor_benchmark.subprocess.check_output',return_value=str((self.app/'Contents/MacOS/Finalverse Test').resolve())):
            with self.assertRaises(ValueError): self.run_preflight()
        (self.root/'new-evidence').mkdir()
        with self.assertRaises(ValueError): self.run_preflight()
    def test_reject_wrong_region_and_linked_session(self):
        self.fixture['region_id']='00000000-0000-0000-0000-000000000000';self.write(self.spec,self.fixture)
        with self.assertRaises(ValueError): self.run_preflight()
        self.fixture['region_id']=REGION_ID;self.write(self.spec,self.fixture)
        other=self.root/'other.xml';self.session.rename(other);self.session.symlink_to(other)
        with self.assertRaises(ValueError): self.run_preflight()

if __name__=='__main__': unittest.main()
