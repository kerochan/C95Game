# include <Siv3D.hpp> // OpenSiv3D v0.2.8
# include <HamFramework.hpp>
#include <filesystem>
#include <fstream>

 
#define byte char

namespace GameInfo {



}

namespace Pathes {
	const String title = U"Resources/Pictures/title.png";
	const String begin = U"Resources/Pictures/start.png";
	const String inst = U"Resources/Pictures/inst.png";

	const String rem_pers = U"Resources/Pictures/leftnum.png";
	const String elp_time = U"Resources/Pictures/time.png";
	const String score = U"Resources/Pictures/score.png";
	const String walk_start = U"Resources/Pictures/walk_start.png";
	const String player = U"Resources/Pictures/player.png";

	const std::array<const String, 16> road_pathes = {
		U"Resources/Pictures/road_0.png",
		U"Resources/Pictures/road_1.png",
		U"Resources/Pictures/road_2.png",
		U"Resources/Pictures/road_3.png",
		U"Resources/Pictures/road_4.png",

		U"Resources/Pictures/road_5.png",
		U"Resources/Pictures/road_6.png",
		U"Resources/Pictures/road_7.png",
		U"Resources/Pictures/road_8.png",
		U"Resources/Pictures/road_9.png",

		U"Resources/Pictures/road_10.png",
		U"Resources/Pictures/road_11.png",
		U"Resources/Pictures/road_12.png",
		U"Resources/Pictures/road_13.png",
		U"Resources/Pictures/road_14.png",

		U"Resources/Pictures/road_15.png"
	};

}


inline int div_abs(const int& a, const int& b) {
	if (a < 0) {
		return (a % b) + b;
	}
	else {
		return a % b;
	}

}

//ブロックの種別を表す
enum class Variety {
	Normal,
	Begin,
	End
};

//方角を表す構造体
class Direction {
	bool north, east, south, west;
public:
	Direction(){}
	Direction(const Direction& direction){
		this->north = direction.north;
		this->east = direction.east;
		this->south = direction.south;
	}

	int getDirection(int index) const noexcept {
		switch (index)
		{
		case 0:
			return static_cast<int>(north);
		case 1:
			return castToIntFromBool(east);
		case 2:
			return castToIntFromBool(south);
		case 3:
			return castToIntFromBool(west);
		default:
			return castToIntFromBool(north);
		}
	}

	void setDirection(int index, int val) noexcept{
		switch (index)
		{
		case 0:
			north = static_cast<bool>(val);
		case 1:
			east = static_cast<bool>(val);
		case 2:
			south = static_cast<bool>(val);
		case 3:
			west = static_cast<bool>(val);
		default:
			
		}
	}



private:
	int castToIntFromBool(bool f) const noexcept{
		if (f)
			return 1;
		return 0;
	}
};




class Block{

	Texture* _texture;
	//接続可能な方向
	std::array<int, 4> _connectable_dire;
	//探索済み方向
	std::array<int, 4> _searched_dire;
	//飛び量
	std::array<int, 4> _jump_val;
	//プレイヤーが存在するかどうか
	bool _f_exist;
public:
	//ステージ上でのブロックのインデックス
	int X, Y;

private:
	//移動方向探索
	int search_dire(const int& coming_idx) {
 		int idx = -1;

		for (int i = 0; i < _searched_dire.size(); i++) {
			if (_searched_dire[i] == 1) {
				idx = i;
				break;
			}	
		}
		if (idx < 0) {
			_searched_dire[(coming_idx + 1) % 4] = 1;
			return (coming_idx + 1) % 4;
		}
		else {
			for (int i = (idx + 1) % 4; i != idx % 4; i = (i + 1) % 4) {
				if (_searched_dire[i] == 0) {
					_searched_dire[i] = _searched_dire[div_abs(i - 1, 4)] + 1;
					if (_searched_dire[div_abs(i - 1, 4)] + 1 == 4) {
						_searched_dire.fill(0);
					}
					//確実にここで1以上の値を返す
					return i;
				}

			}
			
		}
		//エラーの場合
		return -1;
	}

	//特殊な方向制限
	virtual bool spec_dire(const int& idx) {
		return true;
	}

public:
	Block(){
		_texture = new Texture(Pathes::road_pathes[0]);
		_searched_dire.fill(0);
	}

	Block(const int x, const int y, const byte n, const byte e, const byte s, const byte w) {
		X = x; Y = y;
		_connectable_dire = std::move(std::array<int, 4>{n, e, s, w});
		_searched_dire.fill(0);
		_texture = new Texture(Pathes::road_pathes[8 * n + 4 * e + 2 * s + w]);
	}



	void set_connectable_dire(const int& idx, const int& val) {
		_connectable_dire[idx] = val;
	}

	//移動方向の計算など
	virtual void update() {

	}

	//アニメーションの描画
	virtual void draw_animation() const {
		this->draw(X * this->width(), Y * this->height());
	}

	//テスト用の関数
	int bottom(int);


	
};


int Block::bottom(int cidx) {
	return Block::search_dire(0);
}

///////////////////////////////////////////////////////////////////////
//
//  プレイヤーのジャンプ動作
//
class PlayerJumping {


};


class Player : public Texture{

	Vector2D<int> _pos;


public:
	Player(){}

	Player(const int x, const int y) : Texture(Pathes::player) {
		_pos.x = x; _pos.y = y;

	}
	
	void update() {

	}


	void draw_animation() const{
		
		this->draw(_pos);
	}

private:


};




class Stage {
	Grid<Block*> _stage;
	//ステージ番号
	int _stage_num;

	//ステージが始まったかどうかのフラグ
	bool _f_started;
	//ステージが終わったかどうかのフラグ
	bool _f_ended;

	Player* _player;
	//stageの親のworldオブジェクト
	World* _parent_world;
private:

public:
	Stage(){
		_f_started = false;
		_f_ended = false;
		_parent_world = nullptr;
		_player = nullptr;
	}
	Stage(World* world) : Stage(){
		_parent_world = world;
	}

	void setBlock(const int Width, const int Height, Block* block) {
		_stage[Height][Width] = block;
	}

	const Block* getBlock(const int Width, const int Height) {
		return _stage[Width][Height];
	}

	void update() {
		//ゲームが進行中ならば
		if (_f_started) {

		}
	}

	void draw() const {
		
	}

};




//ステージの遷移などを行う
class World {
	
	const int WORLD_SIZE;

	int _playing_stage_num;
	Array<Stage*> _world;

public:

	World(int size  = 0) : WORLD_SIZE(size){
		_playing_stage_num = 0;
	}

	Stage* getStage(int index) const {
		if(isRange(index))
			return _world[index];
	}

	int getCurrentStage() const noexcept {
		return _playing_stage_num;
	}

	void setStage(Stage* stage, int index) {
		if (isRange(index))
			_world[index] = stage;
	}

private:
	bool isRange(int num) const noexcept {
		return 0 <= num && num < WORLD_SIZE;
	}


};


///////////////////////////////////////////////////////////////////////
//
//  タイトル画面のロゴ
//

class TitleTexture : public Texture{
	int _time;
	int _count;
	int _X, _Y;
	double _i_v;
	int _i_y = -100;
	const int _count_max = 5;
	const int _h = 100;
	const double _g = 0.2;
	const double _e = 0.8;
	const double _min_v = 5.0;
public:
	
	TitleTexture() : Texture(Pathes::title){
		_count = 0;
		_time = 0;
		_X = 320; _Y = _i_y;
		_i_v = 0;
	}


	void update_d() {
		if (_count < _count_max) {
			_time++;
		}
		
		_Y = static_cast<int>(_g * (_time * _time) / 2 + _i_v * _time + _i_y);
		if (_Y > _h ) {
			_count++;
			_i_y = _h;
			_i_v = -_e * (_g * _time + _i_v);
			_time = 0;
			
		}
	}

	void draw_d() const {
		this->drawAt(_X, _Y);
	}
};

///////////////////////////////////////////////////////////////////////
//
//  ゲーム画面で表示される枠
//

class GameFrame {
	Rect _rec;
	const Point _rec_pos = {500, 0};
	const int _rec_width = 140;
	const int _rec_height = 480;
	Texture _rem_pers;
	Texture _elp_time;
	Texture _score;

	int _rem_val;
	int _elp_time_val;
	int _score_val;

public:
	GameFrame() {
		_rec.setPos(_rec_pos);
		_rec.setSize(_rec_width, _rec_height);

		_rem_pers = Texture(Pathes::rem_pers);
		_elp_time = Texture(Pathes::elp_time);
		_score = Texture(Pathes::score);

	}

	void draw() const {
		_rec.draw(Palette::Green);
		_rem_pers.drawAt(_rec.pos.x + _rec_width / 2, 200);
		_elp_time.drawAt(_rec.pos.x + _rec_width / 2, 300);
		_score.drawAt(_rec.pos.x + _rec_width / 2, 400);
	}
};

///////////////////////////////////////////////////////////////////////
//
//  ファイルへのアクセスなど
//
class FileOperator {
	std::string _directory_path;
	Array<std::filesystem::path> _filepath_list;

public:
	FileOperator(){}

	FileOperator(std::string path){
		_directory_path = path;
		createFilePathListInConstructor();
	}

	//コンストラクタで指定したディレクトリに存在するすべてのファイルのパスを得る
	const Array<std::filesystem::path>& getFilePathes() {
		return this->_filepath_list;
	}
private:
	//constructor内で、指定したディレクトリに存在するファイルのパスのリストを生成する
	void createFilePathListInConstructor() {
		try {
			std::filesystem::directory_iterator end_itr;
			std::filesystem::directory_iterator it_itr(_directory_path);
			for (; it_itr != end_itr; ++it_itr) {
				if ((*it_itr).is_regular_file()) {
					this->_filepath_list.push_back((*it_itr).path());
				}
			}
		}
		catch (const std::filesystem::filesystem_error& e) {
			throw e;
		}
		
	}
};



///////////////////////////////////////////////////////////////////////
//
//  全てのシーンからアクセスできるデータ
//
struct GameData
{

};

using MyApp = SceneManager<String, GameData>;

///////////////////////////////////////////////////////////////////////
//
//  タイトル
//
class Title : public MyApp::Scene {
	TitleTexture _title;
	Texture _begin;
	const Vector2D<int> _vec_begin = { Window::Size().x / 2, 300 };
	Texture _inst;
	const Vector2D<int> _vec_inst = { Window::Size().x / 2, 400 };

public:
	Title(const InitData& init)
		: IScene(init) 
	{
		_begin = Texture(Pathes::begin);
		_inst = Texture(Pathes::inst);
	}

	void update() override {
		_title.update_d();
		if (_begin.regionAt(_vec_begin).leftClicked()) {
			changeScene(U"Game");
		}
		else if (_inst.regionAt(_vec_inst).leftClicked()) {
			changeScene(U"Instruct");
		}
	}

	void draw() const override {
		_title.draw_d();
		_begin.drawAt(_vec_begin);
		_inst.drawAt(_vec_inst);
	}
}; 

///////////////////////////////////////////////////////////////////////
//
//  操作説明
//
class Instruct : public MyApp::Scene {
public:
	Instruct(const InitData& init)
		: IScene(init)
	{
		
	}

	void update() override {

	}

	void draw() const override {

	}
};

///////////////////////////////////////////////////////////////////////
//
//  ゲーム
//
class Game : public MyApp::Scene {

	enum GameState {
		pre,
		main
	} _state;

	World _world;
	GameFrame frame;


	Texture _walk_start;
	const Vector2D<int> _vec_walk = { Window::Size().x / 2, 300 };
private:
	void read_stage_data() {

		Array<std::filesystem::path> stage_datas;
		try {
			int index = 0;
			stage_datas = FileOperator("Resources/stagedata").getFilePathes();

			//ステージファイルの内容を読み込む
			for each (auto path in stage_datas)
			{
				
				std::ifstream ifs(path, std::ios_base::binary);
				if (ifs) {
					byte temp_w, temp_h, data;
					//ステージの幅と高さを読み取る
					ifs.read(&temp_w, 1); ifs.read(&temp_h, 1);
					Stage* temp_stage = new Stage();
					for (int y = 0; y < temp_h; y++) {
						for (int x = 0; x < temp_w; x++) {
							ifs.read(&data, 1);
							Variety var = static_cast<Variety>(data >> 4);
							byte dire = data & 0b1111;
							temp_stage->setBlock(x, y, new Block());
							switch (var)
							{
							case Variety::Normal:
								
								break;
							case Variety::Begin:
								
								temp_stage->player = Player(x * temp_stage.get_block_data(x, y).width(), y * temp_stage.get_block_data(x, y).height());
								break;
							case Variety::End:
								temp_stage.set_block_data(x, y, Block(x, y, dire >> 3, (dire >> 2) & 1, (dire >> 1) & 1, dire & 1));
								break;
							default:
								break;
							}
						}
					}
					_world.setStage(&temp_stage, index);
				}
				ifs.close();
				index++;
			}


		}
		catch (const std::filesystem::filesystem_error& e) {

		}

	}

public:
	Game(const InitData& init)
		: IScene(init)
	{
		_state = pre;
		_walk_start = Texture(Pathes::walk_start);
		read_stage_data();
	}

	void update() override {
		switch (_state)
		{
		case pre:
			if (_walk_start.regionAt(_vec_walk).leftClicked()) {
				_state = main;
			}
			break;
		case main:




			break;
		default:
			break;
		}
	}

	void draw() const override {
		frame.draw();
		_world.getStage(_world.getCurrentStage)->draw();
		switch (_state)
		{
		case pre:
			_walk_start.drawAt(_vec_walk);
			
			break;
		case main:

			

			break;
		default:
			break;
		}
	}
};



void Main()
{

	MyApp manager;

	manager.add<Title>(U"Title");
	manager.add<Instruct>(U"Instruct");
	manager.add<Game>(U"Game");

	while (System::Update())
	{
		if (!manager.update()) {
			break;
		}
	}
}
