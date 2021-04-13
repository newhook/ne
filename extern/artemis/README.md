AtermisCPP is an attempt to port the Artemis framework, written in Java, to C++
>[The Artemis Java framework](http://gamadu.com/artemis/ "Title")


![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")21/25  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"Component"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"ComponentMapper"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"ComponentType"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"ComponentTypeManager"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"Entity"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"EntityProcessingSystem"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"EntitySystem"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"SystemBitManager"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"ImmutableBag"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"EntityManager"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"DelayedEntityProcessingSystem"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"DelayedEntitySystem"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"GroupManager"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"IntervalEntityProcessingSystem"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"IntervalEntitySystem"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"Manager"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"SystemManager"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"TagManager"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"utils"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"World"  
![alt text](https://dl.dropbox.com/u/12043338/11check_mark.png "check")"Bag"  
[ ]"FastMath"  
[ ]"Timer"  
[ ]"TrigLUT"  
[ ]"Utils" 
 
***

>Note:  
>This project is created with CodeLite.   
>Working with GCC(mingw) 4.7.0; Enabled experimental c++11 with -std=c++11 (It requires the new standard features)  
>Documentation and whatnot will be done when every component is in place and working.

***

example:

Our Movement component ( should probably called VelocityComponent) And our Position component.  

	class MovementComponent : public artemis::Component
	{
	public:
		float velocityX;
		float velocityY;
		
		MovementComponent(float velocityX, float velocityY)
		{
			this->velocityX = velocityX;
			this->velocityY = velocityY;
		};
	};
	
	class PositionComponent : public artemis::Component
    {
		
	public:
		float posX;
		float posY;
		PositionComponent(float posX, float posY)
		{
			this->posX = posX;
			this->posY = posY;
		};
	};
	
Our system that allows for he positions to be updated  
				
	class MovementSystem : public artemis::EntityProcessingSystem
	{
	private:
		artemis::ComponentMapper<MovementComponent> velocityMapper;
		artemis::ComponentMapper<PositionComponent> positionMapper;

	public:
		MovementSystem() {
			setComponentTypes<MovementComponent,PositionComponent>();
		};

		virtual void initialize() {
			velocityMapper.init(*world);
			positionMapper.init(*world);
		};

		virtual void processEntity(artemis::Entity &e) {
			positionMapper.get(e)->posX += velocityMapper.get(e)->velocityX * world->getDelta();
			positionMapper.get(e)->posY += velocityMapper.get(e)->velocityY * world->getDelta();
		};

	};

Our test run  

	int main(int argc, char **argv) {
		
		artemis::World world;
		artemis::SystemManager * sm = world.getSystemManager();
		MovementSystem * movementsys = (MovementSystem*)sm->setSystem(new MovementSystem());
		artemis::EntityManager * em = world.getEntityManager();
		
		sm->initializeAll();
		
		artemis::Entity & player = em->create();
		
		
		
		player.addComponent(new MovementComponent(2,4));
		player.addComponent(new PositionComponent(0,0));
		player.refresh();
		
		PositionComponent * comp = (PositionComponent*)player.getComponent<PositionComponent>();
		
		while(true){
			
			world.loopStart();
			world.setDelta(0.0016f);
			movementsys->process();
			
			std::cout << "X:"<< comp->posX << std::endl;
			std::cout << "Y:"<< comp->posY << std::endl;
			Sleep(160);
		}
		
		return 0;
	}
***


Sidar Talei 2012