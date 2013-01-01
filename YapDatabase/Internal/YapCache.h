#import <Foundation/Foundation.h>

@class YapThreadSafeCache;
@class YapThreadUnsafeCache;

/**
 * YapCache implements a simple strict cache.
 *
 * It is very similar to NSCache and shares a similar API.
 * However, YapCache implements a strict countLimit and monitors usage so eviction is properly ordered.
 *
 * For example:
 * If you set a countLimit of 4, then when you add the 5th item to the cache, another item is automatically evicted.
 * It doesn't happen at a later time as with NSCache. It happens atomically during the addition of the 5th item.
 
 * Which item gets evicted? That depends entirely on usage.
 * YapCache maintains an ordered list based on which keys have been most recently accessed or added.
 * So when you fetch an item from the cache, that item goes to the end of the eviction list.
 * Thus, the item evicted is always the least recently used item.
 *
 * There are 2 versions of YapCache: one that is thread-safe, and another that is not.
 * 
 * - If you are already serializing access to the cache (via a single thread or serial dispatch queue),
 *   then you can use the thread-unsafe version which is slightly faster.
 * - Otherwise you should use the thread-safe version.
**/

@interface YapThreadUnsafeCache : NSObject

/**
 * Initializes a cache.
 *
 * Since the countLimit is a common configuration, it may optionally be passed during initialization.
 * This is als used as hint internally when initializing components (i.e. [NSMutableDictionary initWithCapacity:]).
 *
 * Unless configured otherwise, the cache will be thread-safe.
**/
- (id)initWithKeyClass:(Class)keyClass;
- (id)initWithKeyClass:(Class)keyClass countLimit:(NSUInteger)countLimit;

/**
 * The countLimit specifies the maximum number of items to keep in the cache.
 * This limit is strictly enforced.
 *
 * The default countLimit is 40.
 *
 * You may optionally disable the countLimit by setting it to zero.
 *
 * You may change the countLimit at any time.
 * Changes to the countLimit take immediate effect on the cache (before the set method returns).
 * Thus, if needed, you can temporarily increase the cache size for certain operations.
**/
@property (nonatomic, assign, readwrite) NSUInteger countLimit;

/**
 * When querying the cache for an object via objectForKey,
 * the hitCount is incremented if the object is in the cache,
 * and the missCount is incremented if the object is not in the cache.
**/
@property (nonatomic, readonly) NSUInteger hitCount;
@property (nonatomic, readonly) NSUInteger missCount;

/**
 * When adding objects to the cache via setObject:forKey:,
 * the evictionCount is incremented if the cache is full,
 * and the added object causes another object (the least recently used object) to be evicted.
**/
@property (nonatomic, readonly) NSUInteger evictionCount;

/**
 * The normal cache stuff...
**/

- (void)setObject:(id)object forKey:(id)key;

- (id)objectForKey:(id)key;

- (NSUInteger)count;

- (void)removeAllObjects;
- (void)removeObjectForKey:(id)key;
- (void)removeObjectsForKeys:(NSArray *)keys;

- (NSSet *)keysOfEntriesPassingTest:(BOOL (^)(id key, id obj, BOOL *stop))predicate;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface YapThreadSafeCache : YapThreadUnsafeCache

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface YapCacheCollectionKey : NSObject <NSCopying>

- (id)initWithCollection:(NSString *)collection key:(NSString *)key;

@property (nonatomic, strong, retain) NSString *collection;
@property (nonatomic, strong, retain) NSString *key;

- (BOOL)isEqual:(id)anObject;
- (NSUInteger)hash;

@end