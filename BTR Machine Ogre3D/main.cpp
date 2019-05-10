/*
Управление башней и пушкой с помошью - NUMPAD8, NUMPAD2, NUMPAD4, NUMPAD6.
Выстрел - NUMPAD5.
Езда с помощью стрелок.
Управление камеры - мышь и W,S,A,D.

/////////

Control tower and gun using - NUMPAD8, NUMPAD2, NUMPAD4, NUMPAD6.
Shot - NUMPAD5
Movement is carried out by arrows.
Camera control - mouse move and W, S, A, D.

*/

#pragma warning (disable:4251)
#pragma warning (disable:4193)
#pragma warning (disable:4275)
#pragma warning (disable:4244)

#define OGRE_STATIC_GL
#define OGRE_STATIC_CgProgramManager
#define OGRE_STATIC_ParticleFX
#define OGRE_STATIC_BSPSceneManager

#include <OgreStaticPluginLoader.h>

#include "ExampleApplication.h"

class Example1FrameListener : public Ogre::FrameListener
{
private:
	Ogre::SceneNode* _BaseTop;
	Ogre::SceneNode* _Base;
	Ogre::SceneNode* _BaseGun;

	Ogre::ParticleSystem* _Shot;

	Ogre::SceneNode* _Wheel1l;
	Ogre::SceneNode* _Wheel1r;
	Ogre::SceneNode* _Wheel2l;
	Ogre::SceneNode* _Wheel3l;
	Ogre::SceneNode* _Wheel4l;

	OIS::InputManager* _man;
	OIS::Keyboard* _key;

	Ogre::Camera* _Cam;
	OIS::Mouse* _mouse;

	Ogre::Timer _timerGun;
	Ogre::Timer _timerGunRearm;

	Ogre::PolygonMode _PolyMode;

	int AngleGun = 0; //Угол пушки
	bool GunRearm = 1; //задержка выстрелов
	bool RenderMode = 1; //переключение решима ренлера
	bool TurnLeft = 1; //поворот
	bool TurnRight = 1; //поворот

	int movementSpeed = 50; //скорость передвижения БТР

	Ogre::SceneNode* _SinbadNode1;
	Ogre::Entity* _SinbadEnt1;
	Ogre::SceneNode* _SinbadNode2;
	Ogre::Entity* _SinbadEnt2;
	Ogre::AnimationState* _aniState1;
	Ogre::AnimationState* _aniStateTop1;
	Ogre::AnimationState* _aniState2;
	Ogre::AnimationState* _aniStateTop2;

public:
	Example1FrameListener(Ogre::ParticleSystem* Shot, Ogre::SceneNode* SinbadNode1, Ogre::Entity* ent1, Ogre::SceneNode* SinbadNode2, Ogre::Entity* ent2, Ogre::SceneNode* Base, Ogre::SceneNode* Wheel1l, Ogre::SceneNode* Wheel1r, Ogre::SceneNode* Wheel2l, Ogre::SceneNode* Wheel3l, Ogre::SceneNode* Wheel4l, Ogre::SceneNode* BaseTop, Ogre::SceneNode* BaseGun, RenderWindow* win, Ogre::Camera* cam)
	{
		//бтр
		_BaseTop = BaseTop;
		_BaseGun = BaseGun;
		_Base = Base;

		_Shot = Shot;
		//колеса
		_Wheel1l = Wheel1l;
		_Wheel1r = Wheel1r;
		_Wheel2l = Wheel2l;
		_Wheel3l = Wheel3l;
		_Wheel4l = Wheel4l;
		//сопроводжение
		_SinbadNode1 = SinbadNode1;
		_SinbadEnt1 = ent1;
		_SinbadNode2 = SinbadNode2;
		_SinbadEnt2 = ent2;

		_Cam = cam;

		_timerGun.reset(); //таймер поворота пушки
		_timerGunRearm.reset(); //таймер выстрела

		_PolyMode = PM_SOLID;

		size_t windowHnd = 0;
		std::stringstream windowHndStr;
		win->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		OIS::ParamList pl;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
		_man = OIS::InputManager::createInputSystem(pl);
		_key = static_cast<OIS::Keyboard*>(_man->createInputObject(OIS::OISKeyboard,
			false));
		_mouse = static_cast<OIS::Mouse*>(_man->createInputObject(OIS::OISMouse, false));

		_aniState1 = _SinbadEnt1->getAnimationState("RunBase");
		//_aniState->setEnabled(true);
		_aniState1->setLoop(false);
		_aniStateTop1 = _SinbadEnt1->getAnimationState("RunTop");
		//_aniStateTop->setEnabled(true);
		_aniStateTop1->setLoop(false);
		_aniState2 = _SinbadEnt2->getAnimationState("RunBase");
		_aniState2->setLoop(false);
		_aniStateTop2 = _SinbadEnt2->getAnimationState("RunTop");
		_aniStateTop2->setLoop(false);

	}

	~Example1FrameListener()
	{
		_man->destroyInputObject(_key);
		_man->destroyInputObject(_mouse);
		OIS::InputManager::destroyInputSystem(_man);
	}

	void WheelUpAnim(const Ogre::FrameEvent &_evt)
	{
		_Wheel1l->pitch(Ogre::Degree(4 * -movementSpeed * _evt.timeSinceLastFrame));
		_Wheel1r->pitch(Ogre::Degree(4 * -movementSpeed * _evt.timeSinceLastFrame));
		_Wheel2l->pitch(Ogre::Degree(4 * -movementSpeed * _evt.timeSinceLastFrame));
		_Wheel3l->pitch(Ogre::Degree(4 * -movementSpeed * _evt.timeSinceLastFrame));
		_Wheel4l->pitch(Ogre::Degree(4 * -movementSpeed * _evt.timeSinceLastFrame));
	};

	void WheelDownAnim(const Ogre::FrameEvent &_evt)
	{
		_Wheel1l->pitch(Ogre::Degree(2 * movementSpeed *_evt.timeSinceLastFrame));
		_Wheel1r->pitch(Ogre::Degree(2 * movementSpeed *_evt.timeSinceLastFrame));
		_Wheel2l->pitch(Ogre::Degree(2 * movementSpeed *_evt.timeSinceLastFrame));
		_Wheel3l->pitch(Ogre::Degree(2 * movementSpeed *_evt.timeSinceLastFrame));
		_Wheel4l->pitch(Ogre::Degree(2 * movementSpeed *_evt.timeSinceLastFrame));
	};

	bool frameStarted(const Ogre::FrameEvent &evt)
	{
		Ogre::Vector3 translate(0, 0, 0); //положение камеры

		_key->capture();
		_mouse->capture();

		_aniState1->addTime(evt.timeSinceLastFrame); //время анимации
		_aniStateTop1->addTime(evt.timeSinceLastFrame);
		_aniState2->addTime(evt.timeSinceLastFrame);
		_aniStateTop2->addTime(evt.timeSinceLastFrame);

		bool walked = false;

		if (_key->isKeyDown(OIS::KC_ESCAPE))
		{
			return false;
		}
		//Поворот башни
		if (_key->isKeyDown(OIS::KC_NUMPAD8) && _key->isKeyDown(OIS::KC_NUMPAD2))
		{
		}
		else
		if (_key->isKeyDown(OIS::KC_NUMPAD8) && _timerGun.getMilliseconds() > 25)
		{
			_timerGun.reset();
			if (AngleGun <30)
			{
				AngleGun += 1;
				_BaseGun->pitch(Ogre::Degree(-0.5));
			}
		}
		else
		if (_key->isKeyDown(OIS::KC_NUMPAD2) && _timerGun.getMilliseconds() > 25)
		{
			_timerGun.reset();
			if (AngleGun > -6)
			{
				AngleGun -= 1;
				_BaseGun->pitch(Ogre::Degree(0.5));
			}
		}
		if (_key->isKeyDown(OIS::KC_NUMPAD4))
		{
			_BaseTop->roll(Ogre::Degree(30 * evt.timeSinceLastFrame));
		}
		if (_key->isKeyDown(OIS::KC_NUMPAD6))
		{
			_BaseTop->roll(Ogre::Degree(-30 * evt.timeSinceLastFrame));
		}
		//Выстрел
		if (_key->isKeyDown(OIS::KC_NUMPAD5) && _timerGunRearm.getMilliseconds() > 1000)
		{
			_timerGunRearm.reset();
			_BaseGun->detachObject(_Shot);
			_Shot->clear();
			_BaseGun->attachObject(_Shot);
		}
		if (!_key->isKeyDown(OIS::KC_NUMPAD5) && _timerGunRearm.getMilliseconds() > 1000)
		{
			//_timerGunRearm.reset();
			_BaseGun->detachObject(_Shot);
			_Shot->clear();
		}
		//поворот колес
		if (_key->isKeyDown(OIS::KC_LEFT) && TurnLeft == 1)
		{
			_Wheel1l->yaw(Ogre::Degree(15), Ogre::Node::TS_WORLD);
			_Wheel1r->yaw(Ogre::Degree(15), Ogre::Node::TS_WORLD);
			TurnLeft = 0;
		}
		else
		if (_key->isKeyDown(OIS::KC_RIGHT) && TurnRight == 1)
		{
			_Wheel1l->yaw(Ogre::Degree(-15), Ogre::Node::TS_WORLD);
			_Wheel1r->yaw(Ogre::Degree(-15), Ogre::Node::TS_WORLD);
			TurnRight = 0;
		}
		if (!_key->isKeyDown(OIS::KC_LEFT) && TurnLeft == 0)
		{
			_Wheel1l->yaw(Ogre::Degree(-15), Ogre::Node::TS_WORLD);
			_Wheel1r->yaw(Ogre::Degree(-15), Ogre::Node::TS_WORLD);
			TurnLeft = 1;
		}

		if (!_key->isKeyDown(OIS::KC_RIGHT) && TurnRight == 0)
		{
			_Wheel1l->yaw(Ogre::Degree(15), Ogre::Node::TS_WORLD);
			_Wheel1r->yaw(Ogre::Degree(15), Ogre::Node::TS_WORLD);
			TurnRight = 1;
		}

		//вперед
		if (_key->isKeyDown(OIS::KC_LEFT) && _key->isKeyDown(OIS::KC_UP))
		{
			WheelUpAnim(evt);
			_Base->roll(Ogre::Degree(-15 * evt.timeSinceLastFrame));
			_Base->translate(0, -movementSpeed*evt.timeSinceLastFrame, 0, Ogre::Node::TS_LOCAL);
			walked = true; //включение анимации Синбада
		}
		else
		if (_key->isKeyDown(OIS::KC_RIGHT) && _key->isKeyDown(OIS::KC_UP))
		{
			WheelUpAnim(evt);
			_Base->roll(Ogre::Degree(15 * evt.timeSinceLastFrame));
			_Base->translate(0, -movementSpeed*evt.timeSinceLastFrame, 0, Ogre::Node::TS_LOCAL);
			walked = true; //включение анимации Синбада
		}
		else
		if (_key->isKeyDown(OIS::KC_UP))
		{
			WheelUpAnim(evt);
			_Base->translate(0, -movementSpeed*evt.timeSinceLastFrame, 0, Ogre::Node::TS_LOCAL);
			walked = true; //включение анимации Синбада
		}
		//назад
		if (_key->isKeyDown(OIS::KC_LEFT) && _key->isKeyDown(OIS::KC_DOWN))
		{
			WheelDownAnim(evt);
			_Base->roll(Ogre::Degree(15 * evt.timeSinceLastFrame));
			_Base->translate(0, movementSpeed / 2 * evt.timeSinceLastFrame, 0, Ogre::Node::TS_LOCAL);
			walked = true; //включение анимации Синбада
		}
		else
		if (_key->isKeyDown(OIS::KC_RIGHT) && _key->isKeyDown(OIS::KC_DOWN))
		{
			WheelDownAnim(evt);
			_Base->roll(Ogre::Degree(-15 * evt.timeSinceLastFrame));
			_Base->translate(0, movementSpeed / 2 * evt.timeSinceLastFrame, 0, Ogre::Node::TS_LOCAL);
			walked = true; //включение анимации Синбада
		}
		else
		if (_key->isKeyDown(OIS::KC_DOWN))
		{
			WheelDownAnim(evt);
			_Base->translate(0, movementSpeed / 2 * evt.timeSinceLastFrame, 0, Ogre::Node::TS_LOCAL);
			walked = true; //включение анимации Синбада
		}
		//анимация Синбада
		if (walked)
		{
			_aniState1->setEnabled(true);
			_aniStateTop1->setEnabled(true);
			_aniState2->setEnabled(true);
			_aniStateTop2->setEnabled(true);
			if (_aniState1->hasEnded())
			{
				_aniState1->setTimePosition(0.0f);
				_aniState2->setTimePosition(0.0f);
			}
			if (_aniStateTop1->hasEnded())
			{
				_aniStateTop1->setTimePosition(0.0f);
				_aniStateTop2->setTimePosition(0.0f);
			}
		}
		else
		{
			_aniState1->setTimePosition(0.0f);
			_aniState1->setEnabled(false);
			_aniStateTop1->setTimePosition(0.0f);
			_aniStateTop1->setEnabled(false);
			_aniState2->setTimePosition(0.0f);
			_aniState2->setEnabled(false);
			_aniStateTop2->setTimePosition(0.0f);
			_aniStateTop2->setEnabled(false);
		}

		//Камера
		if (_key->isKeyDown(OIS::KC_W))
		{
			translate += Ogre::Vector3(0, 0, -10);
		}
		if (_key->isKeyDown(OIS::KC_S))
		{
			translate += Ogre::Vector3(0, 0, 10);
		}
		if (_key->isKeyDown(OIS::KC_A))
		{
			translate += Ogre::Vector3(-10, 0, 0);
		}
		if (_key->isKeyDown(OIS::KC_D))
		{
			translate += Ogre::Vector3(10, 0, 0);
		}

		if (_key->isKeyDown(OIS::KC_R))
		{
			if (_PolyMode == PM_SOLID && RenderMode == 1)
			{
				_PolyMode = PM_WIREFRAME;
			}
			else if (_PolyMode == PM_WIREFRAME && RenderMode == 1)
			{
				_PolyMode = PM_POINTS;
			}
			else if (_PolyMode == PM_POINTS && RenderMode == 1)
			{
				_PolyMode = PM_SOLID;
			}
			_Cam->setPolygonMode(_PolyMode);
			RenderMode = 0;
		}
		if (!_key->isKeyDown(OIS::KC_R))
		{
			RenderMode = 1;
		}

		float rotX = _mouse->getMouseState().X.rel * -0.001f;
		float rotY = _mouse->getMouseState().Y.rel * -0.001f;
		_Cam->yaw(Ogre::Radian(rotX));
		_Cam->pitch(Ogre::Radian(rotY));
		_Cam->moveRelative(translate*evt.timeSinceLastFrame * 10);

		return true;
	}
};

class Example1 : public ExampleApplication
{
private:
	Ogre::SceneNode* Base;
	Ogre::SceneNode* BaseTop;
	Ogre::SceneNode* BaseGun;

	Ogre::ParticleSystem* Shot;

	Ogre::SceneNode* Wheel1l;
	Ogre::SceneNode* Wheel1r;
	Ogre::SceneNode* Wheel2l;
	Ogre::SceneNode* Wheel3l;
	Ogre::SceneNode* Wheel4l;

	Ogre::SceneNode* _SinbadNode1;
	Ogre::Entity* _SinbadEnt1;
	Ogre::SceneNode* _SinbadNode2;
	Ogre::Entity* _SinbadEnt2;

	Ogre::FrameListener* FrameListener;

public:
	Example1()
	{
		FrameListener = NULL;
	}

	~Example1()
	{
		if (FrameListener)
		{
			delete FrameListener;
		}
	}

	void createScene()
	{
		//плоскость
		Ogre::Plane plane(Vector3::UNIT_Y, -10);
		Ogre::MeshManager::getSingleton().createPlane("plane",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
			2000, 2000, 20, 20, true, 1, 5, 5, Vector3::UNIT_Z);
		Ogre::Entity* ent = mSceneMgr->createEntity("LightPlaneEntity", "plane");
		mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
		ent->setMaterialName("Examples/BeachStones");
		//SkyBox
		mSceneMgr->setSkyBox(true, "Examples/EarlyMorningSkyBox", 10);
		//освещение
		Ogre::SceneNode* node = mSceneMgr->createSceneNode("Node1");
		mSceneMgr->getRootSceneNode()->addChild(node);
		Ogre::Light* light1 = mSceneMgr->createLight("Light1");
		light1->setType(Ogre::Light::LT_POINT);
		light1->setPosition(200, 200, -50);
		light1->setDiffuseColour(1.0f, 1.0f, 1.0f);
		//Тень
		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

		//БТР
		Ogre::Entity* b1 = mSceneMgr->createEntity("btr1", "BtrBase.mesh");
		Base = node->createChildSceneNode("Base");
		Base->attachObject(b1);
		Base->setPosition(0, 0, 0);
		Base->yaw(Ogre::Degree(60));
		Base->setScale(15, 10, 10);
		Base->pitch(Ogre::Degree(90));
		b1->setMaterialName("BtrBase");
		//b1->setCastShadows(true);

		//башня
		Ogre::Entity* b2 = mSceneMgr->createEntity("btr2", "BtrTop.mesh");
		BaseTop = Base->createChildSceneNode("BaseTop");
		BaseTop->attachObject(b2);
		BaseTop->setPosition(0, -0.1, -2.93);
		BaseTop->scale(1.1, 1.5, 1.2);
		BaseTop->pitch(Ogre::Degree(180));
		BaseTop->roll(Ogre::Degree(-180)); //поворот башни "y"
		//b2->setMaterialName("BtrTop");

		//Пушка
		Ogre::Entity* b3 = mSceneMgr->createEntity("btr3", "Gun.mesh");
		BaseGun = BaseTop->createChildSceneNode("BaseGun");
		BaseGun->attachObject(b3);
		BaseGun->setPosition(0, -0.70, 0.16);
		BaseGun->scale(0.08, 0.06, 0.1);
		//BaseGun->pitch(Ogre::Degree(-15));

		//выстрел
		Shot = mSceneMgr->createParticleSystem("Shot", "MyShot");



		//колеса
		Ogre::Entity* b1l = mSceneMgr->createEntity("wheel1l", "Wheel.mesh");
		Wheel1l = Base->createChildSceneNode("Wheel1l");
		Wheel1l->attachObject(b1l);
		Wheel1l->setPosition(-1.29028, -1.915664, 0.04332);
		Wheel1l->scale(1, 1.3, 1);
		Wheel1l->scale(1, 0.8, 0.8);

		Ogre::Entity* b1r = mSceneMgr->createEntity("wheel1r", "Wheel.mesh");
		Wheel1r = Base->createChildSceneNode("Wheel1r");
		Wheel1r->attachObject(b1r);
		Wheel1r->setPosition(1.29028, -1.915664, 0.04332);
		Wheel1r->scale(1, 1.3, 1);
		Wheel1r->scale(1, 0.8, 0.8);

		Ogre::Entity* b2l = mSceneMgr->createEntity("wheel2l", "Wheel.mesh");
		Wheel2l = Base->createChildSceneNode("Wheel2l");
		Wheel2l->attachObject(b2l);
		Wheel2l->setPosition(-1.29028, 0.16492, 0.04332);
		Wheel2l->scale(1, 1.3, 1);
		Wheel2l->scale(1, 0.8, 0.8);

		Ogre::Entity* b2r = mSceneMgr->createEntity("wheel2r", "Wheel.mesh");
		Ogre::SceneNode* Wheel2r = Wheel2l->createChildSceneNode("Wheel2r");
		Wheel2r->attachObject(b2r);
		Wheel2r->setPosition(2.58056, 0, 0);

		Ogre::Entity* b3l = mSceneMgr->createEntity("wheel3l", "Wheel.mesh");
		Wheel3l = Base->createChildSceneNode("Wheel3l");
		Wheel3l->attachObject(b3l);
		Wheel3l->setPosition(-1.29028, 2.91186, 0.04332);
		Wheel3l->scale(1, 1.3, 1);
		Wheel3l->scale(1, 0.8, 0.8);

		Ogre::Entity* b3r = mSceneMgr->createEntity("wheel3r", "Wheel.mesh");
		Ogre::SceneNode* Wheel3r = Wheel3l->createChildSceneNode("Wheel3r");
		Wheel3r->attachObject(b3r);
		Wheel3r->setPosition(2.58056, 0, 0);

		Ogre::Entity* b4l = mSceneMgr->createEntity("wheel4l", "Wheel.mesh");
		Wheel4l = Base->createChildSceneNode("Wheel4l");
		Wheel4l->attachObject(b4l);
		Wheel4l->setPosition(-1.29028, 4.99506, 0.04332);
		Wheel4l->scale(1, 1.3, 1);
		Wheel4l->scale(1, 0.8, 0.8);

		Ogre::Entity* b4r = mSceneMgr->createEntity("wheel4r", "Wheel.mesh");
		Ogre::SceneNode* Wheel4r = Wheel4l->createChildSceneNode("Wheel4r");
		Wheel4r->attachObject(b4r);
		Wheel4r->setPosition(2.58056, 0, 0);

		//сопровождение
		_SinbadEnt1 = mSceneMgr->createEntity("f1", "Sinbad.mesh");
		_SinbadNode1 = Base->createChildSceneNode("BaseFollow1");
		_SinbadNode1->attachObject(_SinbadEnt1);
		_SinbadNode1->setPosition(3, 2, -0.5);
		_SinbadNode1->setScale(0.0667, 0.1, 0.1);
		_SinbadNode1->scale(3, 3, 3);
		_SinbadNode1->pitch(Ogre::Degree(90));
		_SinbadNode1->roll(Ogre::Degree(180));
		Ogre::Entity* sword1 = mSceneMgr->createEntity("Sword1", "Sword.mesh");
		_SinbadEnt1->attachObjectToBone("Handle.L", sword1);

		_SinbadEnt2 = mSceneMgr->createEntity("f2", "Sinbad.mesh");
		_SinbadNode2 = Base->createChildSceneNode("BaseFollow2");
		_SinbadNode2->attachObject(_SinbadEnt2);
		_SinbadNode2->setPosition(-3, 2, -0.5);
		_SinbadNode2->setScale(0.0667, 0.1, 0.1);
		_SinbadNode2->scale(3, 3, 3);
		_SinbadNode2->pitch(Ogre::Degree(90));
		_SinbadNode2->roll(Ogre::Degree(180));
		Ogre::Entity* sword2 = mSceneMgr->createEntity("Sword2", "Sword.mesh");
		_SinbadEnt2->attachObjectToBone("Handle.R", sword2);

	}

	void createFrameListener()
	{
		FrameListener = new Example1FrameListener(Shot, _SinbadNode1, _SinbadEnt1, _SinbadNode2, _SinbadEnt2, Base, Wheel1l, Wheel1r, Wheel2l, Wheel3l, Wheel4l, BaseTop, BaseGun, mWindow, mCamera);
		mRoot->addFrameListener(FrameListener);
	}

	void createCamera()
	{
		mCamera = mSceneMgr->createCamera("MyCamera1");
		mCamera->setPosition(0, 100, 100);
		mCamera->lookAt(0, 0, 0);
		mCamera->setNearClipDistance(5);
	}
};


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
{
	Example1 app = Example1();
	app.go();

	return 0;
}